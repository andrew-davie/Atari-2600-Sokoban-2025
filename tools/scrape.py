from bs4 import BeautifulSoup
from bs4.element import NavigableString

import requests
import time


def get_value(s, key):
    for line in s.splitlines():
        if line.startswith(key + ":"):
            return int(line.split(":")[1].strip())
    return None  # Not found

def compress_repeats(s):
    if not s:
        return ""

    result = []
    prev = s[0]
    count = 1

    for c in s[1:]:
        if c == prev:
            count += 1
        else:
            if count > 1:
                result.append(f"{count}{prev}")
            else:
                result.append(prev)
            prev = c
            count = 1

    # Append the last sequence
    if count > 1:
        result.append(f"{count}{prev}")
    else:
        result.append(prev)

    return ''.join(result)

def process_url(part, id, response):
    count = 0
    with open(f'soko_{part}.c', 'w') as f:

        if response.status_code == 200:
            html = response.text
            #    print(html)

            # Load the HTML file
            # with open("scrape.html", "r", encoding="utf-8") as f:
            #     html = f.read()

            soup = BeautifulSoup(html, "html.parser")

            # Find all <div class="searchresult">
            results = soup.find_all("div", class_="searchresult")

            for div in results:
                # Replace <img> tags based on filename
                for img in div.find_all("img"):
                    src = img.get("src", "")
                    replacement = src
                    if "freebox.png" in src:
                        replacement = "$"
                    elif "target.png" in src:
                        replacement = "."
                    elif "empty.png" in src:
                        replacement = " "
                    elif "blank.png" in src:
                        replacement = "="
                    elif "wall.png" in src:
                        replacement = "#"
                    elif "rightplace.png" in src:
                        replacement = "*"
                    elif "with_man_on_target.png" in src:
                        replacement = "+"
                    elif "with_man.png" in src:
                        replacement = "@"

                    #        if replacement != "X":
                    img.replace_with(replacement)

                # Conditionally replace <br> with "|" only if preceded by a "#"
                for br in div.find_all("br"):
                    prev = br.previous_sibling
                    if isinstance(prev, NavigableString) and prev.strip().endswith("#"):
                        br.replace_with("|")
                    else:
                        br.decompose()  # Remove the <br> entirely

                # Get final text output and insert carriage return before 'collection/set'
                output = div.get_text()
                output = output.replace("#=", "#|")
                output = output.replace("Collection/Set", "\nCollection/Set")
                # Print cleaned result
                # print(output)
                # print("-" * 40)

            divs = soup.find_all("div", class_="searchresult")
            for div in divs:
                text = div.get_text()  # get div text with newlines
                meta_data = "meta"
                cutoff_index = text.find("Collection")
                if cutoff_index != -1:
                    after = text.split("Collection", 1)[1]
                    text = text[:cutoff_index]
                    meta_data = after

                width = get_value(meta_data, "Width")
                height = get_value(meta_data, "Height")
                pushes = get_value(meta_data, "Pushes")
                moves = get_value(meta_data, "Moves")

                if width <= 40 and height <= 22:

                    quoted = '\n'.join(f'"{line}"' for line in text.splitlines())
                    clean = quoted.replace('"', '').replace('\n', '|')
                    visual = text.replace('=', ' ')
                    print('#if 0\n{\n/*\n' + visual + '\n*/\n', file=f)

                    print(f"{id},", file=f)
                    id = id + 1
                    count = count+1

                    print(f'{width},{height},{pushes},{moves},', file=f)
                    print('"' + compress_repeats(clean) + '",\n},\n#endif\n', file=f)

                    # print(len(compress_repeats(s)) * 100 / len(s))
                    # print(meta_data)
    return count



part = 1
id = 1
url = 'https://sokoban-solver.com/screens/{}'

while True:
    try:
        response = requests.get(url.format(id))
        response.raise_for_status()  # Raise an error for HTTP 4xx/5xx
        id = id + process_url(part, id, response)
        print(f'{part} complete: id={id}')
        #time.sleep(10)
        part = part + 1


    except requests.exceptions.RequestException as e:
        print(f"Stopping at page {i}: {e}")
        break


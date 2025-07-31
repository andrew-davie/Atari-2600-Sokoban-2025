from bs4 import BeautifulSoup
from bs4.element import NavigableString

# Load the HTML file
with open("scrape.html", "r", encoding="utf-8") as f:
    html = f.read()

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
    print(output)
    print("-" * 40)
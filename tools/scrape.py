from bs4 import BeautifulSoup

# Load the HTML file
with open("yourfile.html", "r", encoding="utf-8") as f:
    html = f.read()

# Parse it with BeautifulSoup
soup = BeautifulSoup(html, "html.parser")

# Find all <div class="searchresult">
results = soup.find_all("div", class_="searchresult")

# Extract the HTML or text content of each matching div
for i, div in enumerate(results):
    # Uncomment one of the two lines below depending on your needs:

    # To get just the inner text (no HTML tags):
    # print(div.get_text(strip=True))

    # To get the full HTML content of the div:
    print(div.prettify())

    print("-" * 40)  # separator for readability
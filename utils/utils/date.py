from datetime import datetime as dt

# Get the current date and time in the desired format
encoded_date_time = dt.now().strftime('%Y%m%d%H%M')

# Write the encoded date and time to the file
with open("date.txt", "w") as file:
    file.write(f'const char *name = "{encoded_date_time}";')

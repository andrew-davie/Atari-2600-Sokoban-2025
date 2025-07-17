def read_serial_number(file_name):
    try:
        with open(file_name, 'r') as file:
            serial_number = int(file.read())
            return serial_number
    except FileNotFoundError:
        # If file doesn't exist, return 0 as initial serial number
        return 0
    except ValueError:
        # If file contains invalid data, return None
        return None


def write_serial_number(file_name, serial_number):
    with open(file_name, 'w') as file:
        file.write(str(serial_number))


def main():
    file_name = "serial_number.txt"

    # Read the current serial number from the file
    current_serial_number = read_serial_number(file_name)

    if current_serial_number is None:
        print("Error: Invalid data in the file.")
        return

    if current_serial_number == 0:
        print("Initial serial number.")

    # Increment the serial number
    new_serial_number = current_serial_number + 1

    # Write the updated serial number back to the file
    write_serial_number(file_name, new_serial_number)

    print("Serial number incremented successfully.")
    print("New serial number:", new_serial_number)


if __name__ == "__main__":
    main()

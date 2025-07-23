import math

def generate_circle_points(radius, divisions, aspect_ratio=1.0):
    points = []
    for i in range(divisions):
        angle = 2 * math.pi * i / divisions
        x = int(round(radius * math.cos(angle) * aspect_ratio))
        y = int(round(radius * math.sin(angle)))
        points.append((x, y))
    return points

# Example usage
if __name__ == "__main__":
    radius = float(input("Enter radius: "))
    divisions = int(input("Enter number of divisions: "))
    aspect_ratio = float(input("Enter x:y aspect ratio (e.g. 2.0): "))

    points = generate_circle_points(radius, divisions, aspect_ratio)

    print("\n// C struct definition")
    print("typedef struct { int x; int y; } Point;")

    print(f"\nPoint circlePoints[{len(points)}] = {{")
    for x, y in points:
        print(f"    {{ {x}, {y} }},")
    print("};")



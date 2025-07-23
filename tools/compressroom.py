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

# Example usage
s = (

"      ######|"
"      #    #|"
"      # @  ###|"
"####  #      #|"
"#  ####..#.#$#####|"
"# $ $ ##...      #|"
"#     .....#$$   #|"
"###### ##$## #####|"
"     #  $    #|"
"     #### ####|"
"       #  #|"
"       #  #####|"
"     ### $    #|"
"     #  $ $   #|"
"     # #$# ####|"
"     #     #|"
"     #######"

     )
print(compress_repeats(s))

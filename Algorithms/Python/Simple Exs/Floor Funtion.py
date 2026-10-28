while True:
    try:
        def floor_func(float_num):
            int_part = 0
            int_part = int(float_num)
            return int_part

        n = float(input("Type a number : "))

        print(floor_func(n))
    except ValueError:
        print("Entry not Valid!")

firstnumber = int(input("Choisie ton premier chifre  :"))
operation = input("Choisie une operation mathematique :")
secondnumber = int(input("Choisie ton deuxieme Chifre :"))

if operation == "+" :
    print(firstnumber + secondnumber)

if operation == "-" :
    print(firstnumber - secondnumber)


if operation == "*" :
    print(firstnumber * secondnumber)


if operation == "/" :
    print(firstnumber / secondnumber)



endsession = input("Veux tu finire cette session? [Y/n]  :")

if endsession == "n" :
    firstnumber = input("Choisie ton premier chifre :")
    operation = input("Choisie une operation mathematique :")
    secondnumber = input("Choisie ton deuxieme Chifre :")


    if operation == "+" :
        print(int(firstnumber) + int(secondnumber))
    if operation == "-" :
        print(int(firstnumber) - int(secondnumber))
    if operation == "*" :
        print(int(firstnumber) * int(secondnumber))
    if operation == "/" :
        print(int(firstnumber) / int(secondnumber))
if endsession == "y":
    print("Bye!")
with open("cloudlink.ino") as main:
    main = main.readlines()

debug = input("Enable debuggin?(y/n): ").lower()
if debug == "n":
    main[main.index("#define DEBUG 1\n")] = "#define DEBUG 0"

main = "".join(main)
with open("cloudlink_out.ino", "w") as out:
    out.write(main)

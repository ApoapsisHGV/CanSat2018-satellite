with open("cloudlink.ino") as main:
    main = main.readlines()

debug = input("Enable debuggin?(y/n): ").lower()
print(main)
if debug == "n":
    main[main.index("#define DEBUG\n")] = "//#define DEBUG"

main = "".join(main)
with open("cloudlink_out.ino", "w") as out:
    out.write(main)

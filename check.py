import Image

f = Image.fromstring("RGB", (1,1), chr(0x55) + chr(0x66) + chr(0x77))
f.save("ref.png")

print Image.open("new.png")
print repr(Image.open("new.png").tostring())

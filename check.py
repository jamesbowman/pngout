import Image

im = Image.open("new.png")
print im
print repr(Image.open("new.png").tostring())

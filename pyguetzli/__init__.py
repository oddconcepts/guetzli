import _guetzli

def Rgb2Jpeg(rgb, width, height, quality=85):
    return _guetzli.RgbToJpeg(rgb, width, height, quality)

def Jpeg2Jpeg(jpeg, quality=85):
    return _guetzli.JpegToJpeg(jpeg, quality)


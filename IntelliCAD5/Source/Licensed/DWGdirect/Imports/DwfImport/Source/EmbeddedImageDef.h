#ifndef __EMBEDDEDIMAGEDEF_H
#define __EMBEDDEDIMAGEDEF_H

class EmbeddedImageDef;
typedef OdSmartPtr<EmbeddedImageDef> EmbeddedImageDefPtr;

class EmbeddedImageDef : public OdDbRasterImageDef
{
public:
  EmbeddedImageDef(){}
  static EmbeddedImageDefPtr createObject()
  {
    return EmbeddedImageDefPtr(new EmbeddedImageDef, kOdRxObjAttach);
  }
  void beginSave(const OdChar* pIntendedName);
  void setExtension(const OdString& ext){ _ext = ext; }
  void setData(OdStreamBufPtr& buf){ _data = buf; }
  void setGiImage(OdGiRasterImagePtr& image){ _image = image; }
private:
  OdString _ext;
  OdStreamBufPtr _data;
  OdGiRasterImagePtr _image;
};


#endif // !defined(__EMBEDDEDIMAGEDEF_H)


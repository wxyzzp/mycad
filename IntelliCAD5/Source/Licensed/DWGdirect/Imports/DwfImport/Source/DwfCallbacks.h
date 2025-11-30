
#define MY_CALLBACK_LIST\
  CALLBACK_MACRO(Author,author)\
  CALLBACK_MACRO(Comments,comments)\
  CALLBACK_MACRO(Copyright,copyright)\
  CALLBACK_MACRO(Creation_Time,creation_time)\
  CALLBACK_MACRO(Creator,creator)\
  CALLBACK_MACRO(Description,description)\
  CALLBACK_MACRO(Source_Creation_Time,source_creation_time)\
  CALLBACK_MACRO(Source_Filename,source_filename)\
  CALLBACK_MACRO(Source_Modification_Time,source_modification_time)\
  CALLBACK_MACRO(Title,title)\
  CALLBACK_MACRO(Modification_Time,modification_time)\
  CALLBACK_MACRO(Keywords,keywords)\
  CALLBACK_MACRO(Subject,subject)\
  CALLBACK_MACRO(Filled_Ellipse,filled_ellipse)\
  CALLBACK_MACRO(Image,image)\
  CALLBACK_MACRO(Layer,layer)\
  CALLBACK_MACRO(Named_View,named_view)\
  CALLBACK_MACRO(View,view)\
  CALLBACK_MACRO(Units,units)\
  CALLBACK_MACRO(Object_Node,object_node)\
  CALLBACK_MACRO(Outline_Ellipse,outline_ellipse)\
  CALLBACK_MACRO(PNG_Group4_Image,png_group4_image)\
  CALLBACK_MACRO(Font,font)\
  CALLBACK_MACRO(Polygon,polygon)\
  CALLBACK_MACRO(Polymarker,polymarker)\
  CALLBACK_MACRO(Polytriangle,polytriangle)\
  CALLBACK_MACRO(Text,text)\
  CALLBACK_MACRO(Viewport,viewport)\
  CALLBACK_MACRO(Gouraud_Polyline,gouraud_polyline)\
  CALLBACK_MACRO(Gouraud_Polytriangle,gouraud_polytriangle)\
  CALLBACK_MACRO(Contour_Set,contour_set)\
  CALLBACK_MACRO(Polyline,polyline)\
  CALLBACK_MACRO(Fill_Pattern,fill_pattern)\
  CALLBACK_MACRO(Color_Map,color_map)\
CALLBACK_MACRO(Plot_Info,plot_info)

//////////////////////////////////////////////////////
// Import callbacks
//
struct DwfCallbackManager
{
  static DwfImporter* _importer;
#define CALLBACK_MACRO(class_name, class_lower) \
  static WT_Result process_##class_lower(WT_##class_name &, WT_File&);
  MY_CALLBACK_LIST
#undef CALLBACK_MACRO
};


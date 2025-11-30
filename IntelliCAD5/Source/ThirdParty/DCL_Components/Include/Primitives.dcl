dialog {
      layout          = vertical;
      is_enabled      = false;
 }
 
 cluster {
     // layout          = horizontal;
 }
 
 radio_cluster {
    // layout          = horizontal;
    //  is_enabled      = true;
 }
 
 tile {
    //  layout          = horizontal;
    //  is_enabled      = true;
 }
 
 text  : tile {
      fixed_height    = true;       // inhibit vertical expansion
 }
 
 image : tile {
    is_tab_stop = false ;
 }
 
 button : tile {
      fixed_height    = true;
 //   is_tab_stop     = true;
 }
 
 image_button : button {
   //  	fixed_height    = false;
   // 	is_tab_stop     = true;
 }
 
 toggle : tile {
	  fixed_height    = true;
  //  is_tab_stop     = true;
 }
 
 radio_button : tile {
	  fixed_height    = true;
  //  is_tab_stop     = true;
 }
 
 list_box : tile {
 //   is_tab_stop     = true;
      height          = 11;
      width           = 10;
}
 
 edit_box : tile {
	  fixed_height    = true;
  //  is_tab_stop     = true;
 }
 
 popup_list : tile {
 //     is_tab_stop     = true;
        fixed_height    = true;
 }
 
 slider : tile {
 //     is_tab_stop     = true;
 }
 
 spacer : tile {
 }

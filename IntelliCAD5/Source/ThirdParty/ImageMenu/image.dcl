imageDlg : dialog {
	label = "Menu Image";
    key = "image";
    : row {
        : column {
	    height = 25;
	    fixed_height = true;
            : list_box {
                key = "list";
                width = 18;
		height = 27;
		fixed_height = true;
                fixed_width = true;
            }
        }
        : column {
	    height = 25;
	    fixed_height =true;
	    : row {
                : image_button {
                    key = "img1";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img2";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img3";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img4";
                    width = 15;
                    height = 5.5;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
            }
	    : row {
                : image_button {
                    key = "img5";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img6";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img7";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img8";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
            }
	    : row {
                : image_button {
                    key = "img9";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img10";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img11";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img12";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
            }
	    : row {
                : image_button {
                    key = "img13";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img14";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img15";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img16";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
            }
	    : row {
                : image_button {
                    key = "img17";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img18";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img19";
                    width = 15;
                    height = 5.3;
		    fixed_height = true;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
                : image_button {
                    key = "img20";
                    width = 15;
                    height = 5.3;
		    fixed_height = rue;
		    fixed_width = true;
                    color = 0;
                    alignment = centered;
                }
            }
        }
    }
    : row {
        : button {
            label = "Previous";
            key = "prev";
            mnemonic = "P";
            width = 12;
	    fixed_width = true;
	    is_enabled = FALSE;
        }
        : button {
            label = "Next";
            key = "next";
            mnemonic = "N";
            width = 12;
	    fixed_width = true;
	    is_enabled = FALSE;
        }
        ok_cancel;
    }
}

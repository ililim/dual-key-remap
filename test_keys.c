SECTION("Key code validation - Control Keys");

KEY_DEF *left_ctrl_key = find_key_def_by_name("LEFT_CTRL");
EXPECT(left_ctrl_key != NULL, "LEFT_CTRL key should be found");
EXPECT(left_ctrl_key->scan_code == 0x001D, "LEFT_CTRL scan code should be 0x001D");
EXPECT(left_ctrl_key->virt_code == 0xA2, "LEFT_CTRL virt code should be 0xA2");

KEY_DEF *right_ctrl_key = find_key_def_by_name("RIGHT_CTRL");
EXPECT(right_ctrl_key != NULL, "RIGHT_CTRL key should be found");
EXPECT(right_ctrl_key->scan_code == 0xE01D, "RIGHT_CTRL scan code should be 0xE01D");
EXPECT(right_ctrl_key->virt_code == 0xA3, "RIGHT_CTRL virt code should be 0xA3");

KEY_DEF *ctrl_key = find_key_def_by_name("CTRL");
EXPECT(ctrl_key != NULL, "CTRL key should be found");
EXPECT(ctrl_key->scan_code == 0x001D, "CTRL scan code should be 0x001D");
EXPECT(ctrl_key->virt_code == 0xA2, "CTRL virt code should be 0xA2");

KEY_DEF *left_shift_key = find_key_def_by_name("LEFT_SHIFT");
EXPECT(left_shift_key != NULL, "LEFT_SHIFT key should be found");
EXPECT(left_shift_key->scan_code == 0x002A, "LEFT_SHIFT scan code should be 0x002A");
EXPECT(left_shift_key->virt_code == 0xA0, "LEFT_SHIFT virt code should be 0xA0");

KEY_DEF *right_shift_key = find_key_def_by_name("RIGHT_SHIFT");
EXPECT(right_shift_key != NULL, "RIGHT_SHIFT key should be found");
EXPECT(right_shift_key->scan_code == 0x0036, "RIGHT_SHIFT scan code should be 0x0036");
EXPECT(right_shift_key->virt_code == 0xA1, "RIGHT_SHIFT virt code should be 0xA1");

KEY_DEF *shift_key = find_key_def_by_name("SHIFT");
EXPECT(shift_key != NULL, "SHIFT key should be found");
EXPECT(shift_key->scan_code == 0x002A, "SHIFT scan code should be 0x002A");
EXPECT(shift_key->virt_code == 0xA0, "SHIFT virt code should be 0xA0");

KEY_DEF *left_alt_key = find_key_def_by_name("LEFT_ALT");
EXPECT(left_alt_key != NULL, "LEFT_ALT key should be found");
EXPECT(left_alt_key->scan_code == 0x0038, "LEFT_ALT scan code should be 0x0038");
EXPECT(left_alt_key->virt_code == 0xA4, "LEFT_ALT virt code should be 0xA4");

KEY_DEF *right_alt_key = find_key_def_by_name("RIGHT_ALT");
EXPECT(right_alt_key != NULL, "RIGHT_ALT key should be found");
EXPECT(right_alt_key->scan_code == 0xE038, "RIGHT_ALT scan code should be 0xE038");
EXPECT(right_alt_key->virt_code == 0xA5, "RIGHT_ALT virt code should be 0xA5");

KEY_DEF *alt_key = find_key_def_by_name("ALT");
EXPECT(alt_key != NULL, "ALT key should be found");
EXPECT(alt_key->scan_code == 0x0038, "ALT scan code should be 0x0038");
EXPECT(alt_key->virt_code == 0xA4, "ALT virt code should be 0xA4");

KEY_DEF *left_win_key = find_key_def_by_name("LEFT_WIN");
EXPECT(left_win_key != NULL, "LEFT_WIN key should be found");
EXPECT(left_win_key->scan_code == 0xE05B, "LEFT_WIN scan code should be 0xE05B");
EXPECT(left_win_key->virt_code == 0x5B, "LEFT_WIN virt code should be 0x5B");

KEY_DEF *right_win_key = find_key_def_by_name("RIGHT_WIN");
EXPECT(right_win_key != NULL, "RIGHT_WIN key should be found");
EXPECT(right_win_key->scan_code == 0xE05C, "RIGHT_WIN scan code should be 0xE05C");
EXPECT(right_win_key->virt_code == 0x5C, "RIGHT_WIN virt code should be 0x5C");

KEY_DEF *win_key = find_key_def_by_name("WIN");
EXPECT(win_key != NULL, "WIN key should be found");
EXPECT(win_key->scan_code == 0xE05B, "WIN scan code should be 0xE05B");
EXPECT(win_key->virt_code == 0x5B, "WIN virt code should be 0x5B");

SECTION("Key code validation - Basic Keys");

KEY_DEF *backspace_key = find_key_def_by_name("BACKSPACE");
EXPECT(backspace_key != NULL, "BACKSPACE key should be found");
EXPECT(backspace_key->scan_code == 0x0E, "BACKSPACE scan code should be 0x0E");
EXPECT(backspace_key->virt_code == 0x08, "BACKSPACE virt code should be 0x08");

KEY_DEF *capslock_key = find_key_def_by_name("CAPSLOCK");
EXPECT(capslock_key != NULL, "CAPSLOCK key should be found");
EXPECT(capslock_key->scan_code == 0x3A, "CAPSLOCK scan code should be 0x3A");
EXPECT(capslock_key->virt_code == 0x14, "CAPSLOCK virt code should be 0x14");

KEY_DEF *enter_key = find_key_def_by_name("ENTER");
EXPECT(enter_key != NULL, "ENTER key should be found");
EXPECT(enter_key->scan_code == 0x1C, "ENTER scan code should be 0x1C");
EXPECT(enter_key->virt_code == 0x0D, "ENTER virt code should be 0x0D");

KEY_DEF *escape_key = find_key_def_by_name("ESCAPE");
EXPECT(escape_key != NULL, "ESCAPE key should be found");
EXPECT(escape_key->scan_code == 0x01, "ESCAPE scan code should be 0x01");
EXPECT(escape_key->virt_code == 0x1B, "ESCAPE virt code should be 0x1B");

KEY_DEF *space_key = find_key_def_by_name("SPACE");
EXPECT(space_key != NULL, "SPACE key should be found");
EXPECT(space_key->scan_code == 0x39, "SPACE scan code should be 0x39");
EXPECT(space_key->virt_code == 0x20, "SPACE virt code should be 0x20");

KEY_DEF *tab_key = find_key_def_by_name("TAB");
EXPECT(tab_key != NULL, "TAB key should be found");
EXPECT(tab_key->scan_code == 0x0F, "TAB scan code should be 0x0F");
EXPECT(tab_key->virt_code == 0x09, "TAB virt code should be 0x09");

KEY_DEF *noop_key = find_key_def_by_name("NOOP");
EXPECT(noop_key != NULL, "NOOP key should be found");
EXPECT(noop_key->scan_code == 0x0, "NOOP scan code should be 0x0");
EXPECT(noop_key->virt_code == 0x0, "NOOP virt code should be 0x0");

SECTION("Key code validation - Arrow Keys");

KEY_DEF *up_key = find_key_def_by_name("UP");
EXPECT(up_key != NULL, "UP key should be found");
EXPECT(up_key->scan_code == 0xE048, "UP scan code should be 0xE048");
EXPECT(up_key->virt_code == 0x26, "UP virt code should be 0x26");

KEY_DEF *left_key = find_key_def_by_name("LEFT");
EXPECT(left_key != NULL, "LEFT key should be found");
EXPECT(left_key->scan_code == 0xE04B, "LEFT scan code should be 0xE04B");
EXPECT(left_key->virt_code == 0x25, "LEFT virt code should be 0x25");

KEY_DEF *right_key = find_key_def_by_name("RIGHT");
EXPECT(right_key != NULL, "RIGHT key should be found");
EXPECT(right_key->scan_code == 0xE04D, "RIGHT scan code should be 0xE04D");
EXPECT(right_key->virt_code == 0x27, "RIGHT virt code should be 0x27");

KEY_DEF *down_key = find_key_def_by_name("DOWN");
EXPECT(down_key != NULL, "DOWN key should be found");
EXPECT(down_key->scan_code == 0xE050, "DOWN scan code should be 0xE050");
EXPECT(down_key->virt_code == 0x28, "DOWN virt code should be 0x28");

SECTION("Key code validation - Function Keys");

KEY_DEF *f1_key = find_key_def_by_name("F1");
EXPECT(f1_key != NULL, "F1 key should be found");
EXPECT(f1_key->scan_code == 0x3B, "F1 scan code should be 0x3B");
EXPECT(f1_key->virt_code == 0x70, "F1 virt code should be 0x70");

KEY_DEF *f2_key = find_key_def_by_name("F2");
EXPECT(f2_key != NULL, "F2 key should be found");
EXPECT(f2_key->scan_code == 0x3C, "F2 scan code should be 0x3C");
EXPECT(f2_key->virt_code == 0x71, "F2 virt code should be 0x71");

KEY_DEF *f3_key = find_key_def_by_name("F3");
EXPECT(f3_key != NULL, "F3 key should be found");
EXPECT(f3_key->scan_code == 0x3D, "F3 scan code should be 0x3D");
EXPECT(f3_key->virt_code == 0x72, "F3 virt code should be 0x72");

KEY_DEF *f4_key = find_key_def_by_name("F4");
EXPECT(f4_key != NULL, "F4 key should be found");
EXPECT(f4_key->scan_code == 0x3E, "F4 scan code should be 0x3E");
EXPECT(f4_key->virt_code == 0x73, "F4 virt code should be 0x73");

KEY_DEF *f5_key = find_key_def_by_name("F5");
EXPECT(f5_key != NULL, "F5 key should be found");
EXPECT(f5_key->scan_code == 0x3F, "F5 scan code should be 0x3F");
EXPECT(f5_key->virt_code == 0x74, "F5 virt code should be 0x74");

KEY_DEF *f6_key = find_key_def_by_name("F6");
EXPECT(f6_key != NULL, "F6 key should be found");
EXPECT(f6_key->scan_code == 0x40, "F6 scan code should be 0x40");
EXPECT(f6_key->virt_code == 0x75, "F6 virt code should be 0x75");

KEY_DEF *f7_key = find_key_def_by_name("F7");
EXPECT(f7_key != NULL, "F7 key should be found");
EXPECT(f7_key->scan_code == 0x41, "F7 scan code should be 0x41");
EXPECT(f7_key->virt_code == 0x76, "F7 virt code should be 0x76");

KEY_DEF *f8_key = find_key_def_by_name("F8");
EXPECT(f8_key != NULL, "F8 key should be found");
EXPECT(f8_key->scan_code == 0x42, "F8 scan code should be 0x42");
EXPECT(f8_key->virt_code == 0x77, "F8 virt code should be 0x77");

KEY_DEF *f9_key = find_key_def_by_name("F9");
EXPECT(f9_key != NULL, "F9 key should be found");
EXPECT(f9_key->scan_code == 0x43, "F9 scan code should be 0x43");
EXPECT(f9_key->virt_code == 0x78, "F9 virt code should be 0x78");

KEY_DEF *f10_key = find_key_def_by_name("F10");
EXPECT(f10_key != NULL, "F10 key should be found");
EXPECT(f10_key->scan_code == 0x44, "F10 scan code should be 0x44");
EXPECT(f10_key->virt_code == 0x79, "F10 virt code should be 0x79");

KEY_DEF *f11_key = find_key_def_by_name("F11");
EXPECT(f11_key != NULL, "F11 key should be found");
EXPECT(f11_key->scan_code == 0x57, "F11 scan code should be 0x57");
EXPECT(f11_key->virt_code == 0x7A, "F11 virt code should be 0x7A");

KEY_DEF *f12_key = find_key_def_by_name("F12");
EXPECT(f12_key != NULL, "F12 key should be found");
EXPECT(f12_key->scan_code == 0x58, "F12 scan code should be 0x58");
EXPECT(f12_key->virt_code == 0x7B, "F12 virt code should be 0x7B");

KEY_DEF *f13_key = find_key_def_by_name("F13");
EXPECT(f13_key != NULL, "F13 key should be found");
EXPECT(f13_key->scan_code == 0x64, "F13 scan code should be 0x64");
EXPECT(f13_key->virt_code == 0x7C, "F13 virt code should be 0x7C");

KEY_DEF *f14_key = find_key_def_by_name("F14");
EXPECT(f14_key != NULL, "F14 key should be found");
EXPECT(f14_key->scan_code == 0x65, "F14 scan code should be 0x65");
EXPECT(f14_key->virt_code == 0x7D, "F14 virt code should be 0x7D");

KEY_DEF *f15_key = find_key_def_by_name("F15");
EXPECT(f15_key != NULL, "F15 key should be found");
EXPECT(f15_key->scan_code == 0x66, "F15 scan code should be 0x66");
EXPECT(f15_key->virt_code == 0x7E, "F15 virt code should be 0x7E");

KEY_DEF *f16_key = find_key_def_by_name("F16");
EXPECT(f16_key != NULL, "F16 key should be found");
EXPECT(f16_key->scan_code == 0x67, "F16 scan code should be 0x67");
EXPECT(f16_key->virt_code == 0x7F, "F16 virt code should be 0x7F");

KEY_DEF *f17_key = find_key_def_by_name("F17");
EXPECT(f17_key != NULL, "F17 key should be found");
EXPECT(f17_key->scan_code == 0x68, "F17 scan code should be 0x68");
EXPECT(f17_key->virt_code == 0x80, "F17 virt code should be 0x80");

KEY_DEF *f18_key = find_key_def_by_name("F18");
EXPECT(f18_key != NULL, "F18 key should be found");
EXPECT(f18_key->scan_code == 0x69, "F18 scan code should be 0x69");
EXPECT(f18_key->virt_code == 0x81, "F18 virt code should be 0x81");

KEY_DEF *f19_key = find_key_def_by_name("F19");
EXPECT(f19_key != NULL, "F19 key should be found");
EXPECT(f19_key->scan_code == 0x6A, "F19 scan code should be 0x6A");
EXPECT(f19_key->virt_code == 0x82, "F19 virt code should be 0x82");

KEY_DEF *f20_key = find_key_def_by_name("F20");
EXPECT(f20_key != NULL, "F20 key should be found");
EXPECT(f20_key->scan_code == 0x6B, "F20 scan code should be 0x6B");
EXPECT(f20_key->virt_code == 0x83, "F20 virt code should be 0x83");

KEY_DEF *f21_key = find_key_def_by_name("F21");
EXPECT(f21_key != NULL, "F21 key should be found");
EXPECT(f21_key->scan_code == 0x6C, "F21 scan code should be 0x6C");
EXPECT(f21_key->virt_code == 0x84, "F21 virt code should be 0x84");

KEY_DEF *f22_key = find_key_def_by_name("F22");
EXPECT(f22_key != NULL, "F22 key should be found");
EXPECT(f22_key->scan_code == 0x6D, "F22 scan code should be 0x6D");
EXPECT(f22_key->virt_code == 0x85, "F22 virt code should be 0x85");

KEY_DEF *f23_key = find_key_def_by_name("F23");
EXPECT(f23_key != NULL, "F23 key should be found");
EXPECT(f23_key->scan_code == 0x6E, "F23 scan code should be 0x6E");
EXPECT(f23_key->virt_code == 0x86, "F23 virt code should be 0x86");

KEY_DEF *f24_key = find_key_def_by_name("F24");
EXPECT(f24_key != NULL, "F24 key should be found");
EXPECT(f24_key->scan_code == 0x6F, "F24 scan code should be 0x6F");
EXPECT(f24_key->virt_code == 0x87, "F24 virt code should be 0x87");

SECTION("Key code validation - Number Keys");

KEY_DEF *key_0_key = find_key_def_by_name("KEY_0");
EXPECT(key_0_key != NULL, "KEY_0 key should be found");
EXPECT(key_0_key->scan_code == 0x0B, "KEY_0 scan code should be 0x0B");
EXPECT(key_0_key->virt_code == 0x30, "KEY_0 virt code should be 0x30");

KEY_DEF *key_1_key = find_key_def_by_name("KEY_1");
EXPECT(key_1_key != NULL, "KEY_1 key should be found");
EXPECT(key_1_key->scan_code == 0x02, "KEY_1 scan code should be 0x02");
EXPECT(key_1_key->virt_code == 0x31, "KEY_1 virt code should be 0x31");

KEY_DEF *key_2_key = find_key_def_by_name("KEY_2");
EXPECT(key_2_key != NULL, "KEY_2 key should be found");
EXPECT(key_2_key->scan_code == 0x03, "KEY_2 scan code should be 0x03");
EXPECT(key_2_key->virt_code == 0x32, "KEY_2 virt code should be 0x32");

KEY_DEF *key_3_key = find_key_def_by_name("KEY_3");
EXPECT(key_3_key != NULL, "KEY_3 key should be found");
EXPECT(key_3_key->scan_code == 0x04, "KEY_3 scan code should be 0x04");
EXPECT(key_3_key->virt_code == 0x33, "KEY_3 virt code should be 0x33");

KEY_DEF *key_4_key = find_key_def_by_name("KEY_4");
EXPECT(key_4_key != NULL, "KEY_4 key should be found");
EXPECT(key_4_key->scan_code == 0x05, "KEY_4 scan code should be 0x05");
EXPECT(key_4_key->virt_code == 0x34, "KEY_4 virt code should be 0x34");

KEY_DEF *key_5_key = find_key_def_by_name("KEY_5");
EXPECT(key_5_key != NULL, "KEY_5 key should be found");
EXPECT(key_5_key->scan_code == 0x06, "KEY_5 scan code should be 0x06");
EXPECT(key_5_key->virt_code == 0x35, "KEY_5 virt code should be 0x35");

KEY_DEF *key_6_key = find_key_def_by_name("KEY_6");
EXPECT(key_6_key != NULL, "KEY_6 key should be found");
EXPECT(key_6_key->scan_code == 0x07, "KEY_6 scan code should be 0x07");
EXPECT(key_6_key->virt_code == 0x36, "KEY_6 virt code should be 0x36");

KEY_DEF *key_7_key = find_key_def_by_name("KEY_7");
EXPECT(key_7_key != NULL, "KEY_7 key should be found");
EXPECT(key_7_key->scan_code == 0x08, "KEY_7 scan code should be 0x08");
EXPECT(key_7_key->virt_code == 0x37, "KEY_7 virt code should be 0x37");

KEY_DEF *key_8_key = find_key_def_by_name("KEY_8");
EXPECT(key_8_key != NULL, "KEY_8 key should be found");
EXPECT(key_8_key->scan_code == 0x09, "KEY_8 scan code should be 0x09");
EXPECT(key_8_key->virt_code == 0x38, "KEY_8 virt code should be 0x38");

KEY_DEF *key_9_key = find_key_def_by_name("KEY_9");
EXPECT(key_9_key != NULL, "KEY_9 key should be found");
EXPECT(key_9_key->scan_code == 0x0A, "KEY_9 scan code should be 0x0A");
EXPECT(key_9_key->virt_code == 0x39, "KEY_9 virt code should be 0x39");

SECTION("Key code validation - Letter Keys");

KEY_DEF *key_a_key = find_key_def_by_name("KEY_A");
EXPECT(key_a_key != NULL, "KEY_A key should be found");
EXPECT(key_a_key->scan_code == 0x1E, "KEY_A scan code should be 0x1E");
EXPECT(key_a_key->virt_code == 0x41, "KEY_A virt code should be 0x41");

KEY_DEF *key_b_key = find_key_def_by_name("KEY_B");
EXPECT(key_b_key != NULL, "KEY_B key should be found");
EXPECT(key_b_key->scan_code == 0x30, "KEY_B scan code should be 0x30");
EXPECT(key_b_key->virt_code == 0x42, "KEY_B virt code should be 0x42");

KEY_DEF *key_c_key = find_key_def_by_name("KEY_C");
EXPECT(key_c_key != NULL, "KEY_C key should be found");
EXPECT(key_c_key->scan_code == 0x2E, "KEY_C scan code should be 0x2E");
EXPECT(key_c_key->virt_code == 0x43, "KEY_C virt code should be 0x43");

KEY_DEF *key_d_key = find_key_def_by_name("KEY_D");
EXPECT(key_d_key != NULL, "KEY_D key should be found");
EXPECT(key_d_key->scan_code == 0x20, "KEY_D scan code should be 0x20");
EXPECT(key_d_key->virt_code == 0x44, "KEY_D virt code should be 0x44");

KEY_DEF *key_e_key = find_key_def_by_name("KEY_E");
EXPECT(key_e_key != NULL, "KEY_E key should be found");
EXPECT(key_e_key->scan_code == 0x12, "KEY_E scan code should be 0x12");
EXPECT(key_e_key->virt_code == 0x45, "KEY_E virt code should be 0x45");

KEY_DEF *key_f_key = find_key_def_by_name("KEY_F");
EXPECT(key_f_key != NULL, "KEY_F key should be found");
EXPECT(key_f_key->scan_code == 0x21, "KEY_F scan code should be 0x21");
EXPECT(key_f_key->virt_code == 0x46, "KEY_F virt code should be 0x46");

KEY_DEF *key_g_key = find_key_def_by_name("KEY_G");
EXPECT(key_g_key != NULL, "KEY_G key should be found");
EXPECT(key_g_key->scan_code == 0x22, "KEY_G scan code should be 0x22");
EXPECT(key_g_key->virt_code == 0x47, "KEY_G virt code should be 0x47");

KEY_DEF *key_h_key = find_key_def_by_name("KEY_H");
EXPECT(key_h_key != NULL, "KEY_H key should be found");
EXPECT(key_h_key->scan_code == 0x23, "KEY_H scan code should be 0x23");
EXPECT(key_h_key->virt_code == 0x48, "KEY_H virt code should be 0x48");

KEY_DEF *key_i_key = find_key_def_by_name("KEY_I");
EXPECT(key_i_key != NULL, "KEY_I key should be found");
EXPECT(key_i_key->scan_code == 0x17, "KEY_I scan code should be 0x17");
EXPECT(key_i_key->virt_code == 0x49, "KEY_I virt code should be 0x49");

KEY_DEF *key_j_key = find_key_def_by_name("KEY_J");
EXPECT(key_j_key != NULL, "KEY_J key should be found");
EXPECT(key_j_key->scan_code == 0x24, "KEY_J scan code should be 0x24");
EXPECT(key_j_key->virt_code == 0x4A, "KEY_J virt code should be 0x4A");

KEY_DEF *key_k_key = find_key_def_by_name("KEY_K");
EXPECT(key_k_key != NULL, "KEY_K key should be found");
EXPECT(key_k_key->scan_code == 0x25, "KEY_K scan code should be 0x25");
EXPECT(key_k_key->virt_code == 0x4B, "KEY_K virt code should be 0x4B");

KEY_DEF *key_l_key = find_key_def_by_name("KEY_L");
EXPECT(key_l_key != NULL, "KEY_L key should be found");
EXPECT(key_l_key->scan_code == 0x26, "KEY_L scan code should be 0x26");
EXPECT(key_l_key->virt_code == 0x4C, "KEY_L virt code should be 0x4C");

KEY_DEF *key_m_key = find_key_def_by_name("KEY_M");
EXPECT(key_m_key != NULL, "KEY_M key should be found");
EXPECT(key_m_key->scan_code == 0x32, "KEY_M scan code should be 0x32");
EXPECT(key_m_key->virt_code == 0x4D, "KEY_M virt code should be 0x4D");

KEY_DEF *key_n_key = find_key_def_by_name("KEY_N");
EXPECT(key_n_key != NULL, "KEY_N key should be found");
EXPECT(key_n_key->scan_code == 0x31, "KEY_N scan code should be 0x31");
EXPECT(key_n_key->virt_code == 0x4E, "KEY_N virt code should be 0x4E");

KEY_DEF *key_o_key = find_key_def_by_name("KEY_O");
EXPECT(key_o_key != NULL, "KEY_O key should be found");
EXPECT(key_o_key->scan_code == 0x18, "KEY_O scan code should be 0x18");
EXPECT(key_o_key->virt_code == 0x4F, "KEY_O virt code should be 0x4F");

KEY_DEF *key_p_key = find_key_def_by_name("KEY_P");
EXPECT(key_p_key != NULL, "KEY_P key should be found");
EXPECT(key_p_key->scan_code == 0x19, "KEY_P scan code should be 0x19");
EXPECT(key_p_key->virt_code == 0x50, "KEY_P virt code should be 0x50");

KEY_DEF *key_q_key = find_key_def_by_name("KEY_Q");
EXPECT(key_q_key != NULL, "KEY_Q key should be found");
EXPECT(key_q_key->scan_code == 0x10, "KEY_Q scan code should be 0x10");
EXPECT(key_q_key->virt_code == 0x51, "KEY_Q virt code should be 0x51");

KEY_DEF *key_r_key = find_key_def_by_name("KEY_R");
EXPECT(key_r_key != NULL, "KEY_R key should be found");
EXPECT(key_r_key->scan_code == 0x13, "KEY_R scan code should be 0x13");
EXPECT(key_r_key->virt_code == 0x52, "KEY_R virt code should be 0x52");

KEY_DEF *key_s_key = find_key_def_by_name("KEY_S");
EXPECT(key_s_key != NULL, "KEY_S key should be found");
EXPECT(key_s_key->scan_code == 0x1F, "KEY_S scan code should be 0x1F");
EXPECT(key_s_key->virt_code == 0x53, "KEY_S virt code should be 0x53");

KEY_DEF *key_t_key = find_key_def_by_name("KEY_T");
EXPECT(key_t_key != NULL, "KEY_T key should be found");
EXPECT(key_t_key->scan_code == 0x14, "KEY_T scan code should be 0x14");
EXPECT(key_t_key->virt_code == 0x54, "KEY_T virt code should be 0x54");

KEY_DEF *key_u_key = find_key_def_by_name("KEY_U");
EXPECT(key_u_key != NULL, "KEY_U key should be found");
EXPECT(key_u_key->scan_code == 0x16, "KEY_U scan code should be 0x16");
EXPECT(key_u_key->virt_code == 0x55, "KEY_U virt code should be 0x55");

KEY_DEF *key_v_key = find_key_def_by_name("KEY_V");
EXPECT(key_v_key != NULL, "KEY_V key should be found");
EXPECT(key_v_key->scan_code == 0x2F, "KEY_V scan code should be 0x2F");
EXPECT(key_v_key->virt_code == 0x56, "KEY_V virt code should be 0x56");

KEY_DEF *key_w_key = find_key_def_by_name("KEY_W");
EXPECT(key_w_key != NULL, "KEY_W key should be found");
EXPECT(key_w_key->scan_code == 0x11, "KEY_W scan code should be 0x11");
EXPECT(key_w_key->virt_code == 0x57, "KEY_W virt code should be 0x57");

KEY_DEF *key_x_key = find_key_def_by_name("KEY_X");
EXPECT(key_x_key != NULL, "KEY_X key should be found");
EXPECT(key_x_key->scan_code == 0x2D, "KEY_X scan code should be 0x2D");
EXPECT(key_x_key->virt_code == 0x58, "KEY_X virt code should be 0x58");

KEY_DEF *key_y_key = find_key_def_by_name("KEY_Y");
EXPECT(key_y_key != NULL, "KEY_Y key should be found");
EXPECT(key_y_key->scan_code == 0x15, "KEY_Y scan code should be 0x15");
EXPECT(key_y_key->virt_code == 0x59, "KEY_Y virt code should be 0x59");

KEY_DEF *key_z_key = find_key_def_by_name("KEY_Z");
EXPECT(key_z_key != NULL, "KEY_Z key should be found");
EXPECT(key_z_key->scan_code == 0x2C, "KEY_Z scan code should be 0x2C");
EXPECT(key_z_key->virt_code == 0x5A, "KEY_Z virt code should be 0x5A");

SECTION("Key code validation - Numpad Keys");

KEY_DEF *num0_key = find_key_def_by_name("NUM0");
EXPECT(num0_key != NULL, "NUM0 key should be found");
EXPECT(num0_key->scan_code == 0x52, "NUM0 scan code should be 0x52");
EXPECT(num0_key->virt_code == 0x60, "NUM0 virt code should be 0x60");

KEY_DEF *num1_key = find_key_def_by_name("NUM1");
EXPECT(num1_key != NULL, "NUM1 key should be found");
EXPECT(num1_key->scan_code == 0x4F, "NUM1 scan code should be 0x4F");
EXPECT(num1_key->virt_code == 0x61, "NUM1 virt code should be 0x61");

KEY_DEF *num2_key = find_key_def_by_name("NUM2");
EXPECT(num2_key != NULL, "NUM2 key should be found");
EXPECT(num2_key->scan_code == 0x50, "NUM2 scan code should be 0x50");
EXPECT(num2_key->virt_code == 0x62, "NUM2 virt code should be 0x62");

KEY_DEF *num3_key = find_key_def_by_name("NUM3");
EXPECT(num3_key != NULL, "NUM3 key should be found");
EXPECT(num3_key->scan_code == 0x51, "NUM3 scan code should be 0x51");
EXPECT(num3_key->virt_code == 0x63, "NUM3 virt code should be 0x63");

KEY_DEF *num4_key = find_key_def_by_name("NUM4");
EXPECT(num4_key != NULL, "NUM4 key should be found");
EXPECT(num4_key->scan_code == 0x4B, "NUM4 scan code should be 0x4B");
EXPECT(num4_key->virt_code == 0x64, "NUM4 virt code should be 0x64");

KEY_DEF *num5_key = find_key_def_by_name("NUM5");
EXPECT(num5_key != NULL, "NUM5 key should be found");
EXPECT(num5_key->scan_code == 0x4C, "NUM5 scan code should be 0x4C");
EXPECT(num5_key->virt_code == 0x65, "NUM5 virt code should be 0x65");

KEY_DEF *num6_key = find_key_def_by_name("NUM6");
EXPECT(num6_key != NULL, "NUM6 key should be found");
EXPECT(num6_key->scan_code == 0x4D, "NUM6 scan code should be 0x4D");
EXPECT(num6_key->virt_code == 0x66, "NUM6 virt code should be 0x66");

KEY_DEF *num7_key = find_key_def_by_name("NUM7");
EXPECT(num7_key != NULL, "NUM7 key should be found");
EXPECT(num7_key->scan_code == 0x47, "NUM7 scan code should be 0x47");
EXPECT(num7_key->virt_code == 0x67, "NUM7 virt code should be 0x67");

KEY_DEF *num8_key = find_key_def_by_name("NUM8");
EXPECT(num8_key != NULL, "NUM8 key should be found");
EXPECT(num8_key->scan_code == 0x48, "NUM8 scan code should be 0x48");
EXPECT(num8_key->virt_code == 0x68, "NUM8 virt code should be 0x68");

KEY_DEF *num9_key = find_key_def_by_name("NUM9");
EXPECT(num9_key != NULL, "NUM9 key should be found");
EXPECT(num9_key->scan_code == 0x49, "NUM9 scan code should be 0x49");
EXPECT(num9_key->virt_code == 0x69, "NUM9 virt code should be 0x69");

KEY_DEF *num_multiply_key = find_key_def_by_name("NUM_MULTIPLY");
EXPECT(num_multiply_key != NULL, "NUM_MULTIPLY key should be found");
EXPECT(num_multiply_key->scan_code == 0x37, "NUM_MULTIPLY scan code should be 0x37");
EXPECT(num_multiply_key->virt_code == 0x6A, "NUM_MULTIPLY virt code should be 0x6A");

KEY_DEF *num_add_key = find_key_def_by_name("NUM_ADD");
EXPECT(num_add_key != NULL, "NUM_ADD key should be found");
EXPECT(num_add_key->scan_code == 0x4E, "NUM_ADD scan code should be 0x4E");
EXPECT(num_add_key->virt_code == 0x6B, "NUM_ADD virt code should be 0x6B");

KEY_DEF *num_subtract_key = find_key_def_by_name("NUM_SUBTRACT");
EXPECT(num_subtract_key != NULL, "NUM_SUBTRACT key should be found");
EXPECT(num_subtract_key->scan_code == 0x4A, "NUM_SUBTRACT scan code should be 0x4A");
EXPECT(num_subtract_key->virt_code == 0x6D, "NUM_SUBTRACT virt code should be 0x6D");

KEY_DEF *num_decimal_key = find_key_def_by_name("NUM_DECIMAL");
EXPECT(num_decimal_key != NULL, "NUM_DECIMAL key should be found");
EXPECT(num_decimal_key->scan_code == 0x53, "NUM_DECIMAL scan code should be 0x53");
EXPECT(num_decimal_key->virt_code == 0x6E, "NUM_DECIMAL virt code should be 0x6E");

KEY_DEF *num_divide_key = find_key_def_by_name("NUM_DIVIDE");
EXPECT(num_divide_key != NULL, "NUM_DIVIDE key should be found");
EXPECT(num_divide_key->scan_code == 0xE035, "NUM_DIVIDE scan code should be 0xE035");
EXPECT(num_divide_key->virt_code == 0x6F, "NUM_DIVIDE virt code should be 0x6F");

KEY_DEF *numlock_key = find_key_def_by_name("NUMLOCK");
EXPECT(numlock_key != NULL, "NUMLOCK key should be found");
EXPECT(numlock_key->scan_code == 0x145, "NUMLOCK scan code should be 0x145");
EXPECT(numlock_key->virt_code == 0x90, "NUMLOCK virt code should be 0x90");

SECTION("Key code validation - Navigation Keys");

KEY_DEF *apps_key = find_key_def_by_name("APPS");
EXPECT(apps_key != NULL, "APPS key should be found");
EXPECT(apps_key->scan_code == 0xE05D, "APPS scan code should be 0xE05D");
EXPECT(apps_key->virt_code == 0x5D, "APPS virt code should be 0x5D");

KEY_DEF *insert_key = find_key_def_by_name("INSERT");
EXPECT(insert_key != NULL, "INSERT key should be found");
EXPECT(insert_key->scan_code == 0xE052, "INSERT scan code should be 0xE052");
EXPECT(insert_key->virt_code == 0x2D, "INSERT virt code should be 0x2D");

KEY_DEF *delete_key = find_key_def_by_name("DELETE");
EXPECT(delete_key != NULL, "DELETE key should be found");
EXPECT(delete_key->scan_code == 0xE053, "DELETE scan code should be 0xE053");
EXPECT(delete_key->virt_code == 0x2E, "DELETE virt code should be 0x2E");

KEY_DEF *home_key = find_key_def_by_name("HOME");
EXPECT(home_key != NULL, "HOME key should be found");
EXPECT(home_key->scan_code == 0xE047, "HOME scan code should be 0xE047");
EXPECT(home_key->virt_code == 0x24, "HOME virt code should be 0x24");

KEY_DEF *end_key = find_key_def_by_name("END");
EXPECT(end_key != NULL, "END key should be found");
EXPECT(end_key->scan_code == 0xE04F, "END scan code should be 0xE04F");
EXPECT(end_key->virt_code == 0x23, "END virt code should be 0x23");

KEY_DEF *page_up_key = find_key_def_by_name("PAGE_UP");
EXPECT(page_up_key != NULL, "PAGE_UP key should be found");
EXPECT(page_up_key->scan_code == 0xE049, "PAGE_UP scan code should be 0xE049");
EXPECT(page_up_key->virt_code == 0x21, "PAGE_UP virt code should be 0x21");

KEY_DEF *page_down_key = find_key_def_by_name("PAGE_DOWN");
EXPECT(page_down_key != NULL, "PAGE_DOWN key should be found");
EXPECT(page_down_key->scan_code == 0xE051, "PAGE_DOWN scan code should be 0xE051");
EXPECT(page_down_key->virt_code == 0x22, "PAGE_DOWN virt code should be 0x22");

SECTION("Key code validation - Media Keys");

KEY_DEF *media_play_pause_key = find_key_def_by_name("MEDIA_PLAY_PAUSE");
EXPECT(media_play_pause_key != NULL, "MEDIA_PLAY_PAUSE key should be found");
EXPECT(media_play_pause_key->scan_code == 0xE022, "MEDIA_PLAY_PAUSE scan code should be 0xE022");
EXPECT(media_play_pause_key->virt_code == 0xB3, "MEDIA_PLAY_PAUSE virt code should be 0xB3");

KEY_DEF *media_next_track_key = find_key_def_by_name("MEDIA_NEXT_TRACK");
EXPECT(media_next_track_key != NULL, "MEDIA_NEXT_TRACK key should be found");
EXPECT(media_next_track_key->scan_code == 0xE019, "MEDIA_NEXT_TRACK scan code should be 0xE019");
EXPECT(media_next_track_key->virt_code == 0xB0, "MEDIA_NEXT_TRACK virt code should be 0xB0");

KEY_DEF *media_prev_track_key = find_key_def_by_name("MEDIA_PREV_TRACK");
EXPECT(media_prev_track_key != NULL, "MEDIA_PREV_TRACK key should be found");
EXPECT(media_prev_track_key->scan_code == 0xE010, "MEDIA_PREV_TRACK scan code should be 0xE010");
EXPECT(media_prev_track_key->virt_code == 0xB1, "MEDIA_PREV_TRACK virt code should be 0xB1");

KEY_DEF *media_stop_key = find_key_def_by_name("MEDIA_STOP");
EXPECT(media_stop_key != NULL, "MEDIA_STOP key should be found");
EXPECT(media_stop_key->scan_code == 0xE024, "MEDIA_STOP scan code should be 0xE024");
EXPECT(media_stop_key->virt_code == 0xB2, "MEDIA_STOP virt code should be 0xB2");

KEY_DEF *volume_mute_key = find_key_def_by_name("VOLUME_MUTE");
EXPECT(volume_mute_key != NULL, "VOLUME_MUTE key should be found");
EXPECT(volume_mute_key->scan_code == 0xE020, "VOLUME_MUTE scan code should be 0xE020");
EXPECT(volume_mute_key->virt_code == 0xAD, "VOLUME_MUTE virt code should be 0xAD");

KEY_DEF *volume_down_key = find_key_def_by_name("VOLUME_DOWN");
EXPECT(volume_down_key != NULL, "VOLUME_DOWN key should be found");
EXPECT(volume_down_key->scan_code == 0xE02E, "VOLUME_DOWN scan code should be 0xE02E");
EXPECT(volume_down_key->virt_code == 0xAE, "VOLUME_DOWN virt code should be 0xAE");

KEY_DEF *volume_up_key = find_key_def_by_name("VOLUME_UP");
EXPECT(volume_up_key != NULL, "VOLUME_UP key should be found");
EXPECT(volume_up_key->scan_code == 0xE030, "VOLUME_UP scan code should be 0xE030");
EXPECT(volume_up_key->virt_code == 0xAF, "VOLUME_UP virt code should be 0xAF");

SECTION("Key code validation - Punctuation Keys");

KEY_DEF *lbracket_key = find_key_def_by_name("LBRACKET");
EXPECT(lbracket_key != NULL, "LBRACKET key should be found");
EXPECT(lbracket_key->scan_code == 0x1A, "LBRACKET scan code should be 0x1A");
EXPECT(lbracket_key->virt_code == 0xDB, "LBRACKET virt code should be 0xDB");

KEY_DEF *rbracket_key = find_key_def_by_name("RBRACKET");
EXPECT(rbracket_key != NULL, "RBRACKET key should be found");
EXPECT(rbracket_key->scan_code == 0x1B, "RBRACKET scan code should be 0x1B");
EXPECT(rbracket_key->virt_code == 0xDD, "RBRACKET virt code should be 0xDD");

KEY_DEF *backslash_key = find_key_def_by_name("BACKSLASH");
EXPECT(backslash_key != NULL, "BACKSLASH key should be found");
EXPECT(backslash_key->scan_code == 0x2B, "BACKSLASH scan code should be 0x2B");
EXPECT(backslash_key->virt_code == 0xDC, "BACKSLASH virt code should be 0xDC");

KEY_DEF *apostrophe_key = find_key_def_by_name("APOSTROPHE");
EXPECT(apostrophe_key != NULL, "APOSTROPHE key should be found");
EXPECT(apostrophe_key->scan_code == 0x28, "APOSTROPHE scan code should be 0x28");
EXPECT(apostrophe_key->virt_code == 0xDE, "APOSTROPHE virt code should be 0xDE");

KEY_DEF *plus_key = find_key_def_by_name("PLUS");
EXPECT(plus_key != NULL, "PLUS key should be found");
EXPECT(plus_key->scan_code == 0x0d, "PLUS scan code should be 0x0d");
EXPECT(plus_key->virt_code == 0xBB, "PLUS virt code should be 0xBB");

KEY_DEF *comma_key = find_key_def_by_name("COMMA");
EXPECT(comma_key != NULL, "COMMA key should be found");
EXPECT(comma_key->scan_code == 0x33, "COMMA scan code should be 0x33");
EXPECT(comma_key->virt_code == 0xBC, "COMMA virt code should be 0xBC");

KEY_DEF *minus_key = find_key_def_by_name("MINUS");
EXPECT(minus_key != NULL, "MINUS key should be found");
EXPECT(minus_key->scan_code == 0x0c, "MINUS scan code should be 0x0c");
EXPECT(minus_key->virt_code == 0xBD, "MINUS virt code should be 0xBD");

KEY_DEF *period_key = find_key_def_by_name("PERIOD");
EXPECT(period_key != NULL, "PERIOD key should be found");
EXPECT(period_key->scan_code == 0x34, "PERIOD scan code should be 0x34");
EXPECT(period_key->virt_code == 0xBE, "PERIOD virt code should be 0xBE");

KEY_DEF *us_semi_key = find_key_def_by_name("US_SEMI");
EXPECT(us_semi_key != NULL, "US_SEMI key should be found");
EXPECT(us_semi_key->scan_code == 0x27, "US_SEMI scan code should be 0x27");
EXPECT(us_semi_key->virt_code == 0xBA, "US_SEMI virt code should be 0xBA");

KEY_DEF *us_slash_key = find_key_def_by_name("US_SLASH");
EXPECT(us_slash_key != NULL, "US_SLASH key should be found");
EXPECT(us_slash_key->scan_code == 0x35, "US_SLASH scan code should be 0x35");
EXPECT(us_slash_key->virt_code == 0xBF, "US_SLASH virt code should be 0xBF");

KEY_DEF *us_tilde_key = find_key_def_by_name("US_TILDE");
EXPECT(us_tilde_key != NULL, "US_TILDE key should be found");
EXPECT(us_tilde_key->scan_code == 0x29, "US_TILDE scan code should be 0x29");
EXPECT(us_tilde_key->virt_code == 0xC0, "US_TILDE virt code should be 0xC0");

SECTION("Key code validation - Other Keys");

KEY_DEF *print_screen_key = find_key_def_by_name("PRINT_SCREEN");
EXPECT(print_screen_key != NULL, "PRINT_SCREEN key should be found");
EXPECT(print_screen_key->scan_code == 0xE037, "PRINT_SCREEN scan code should be 0xE037");
EXPECT(print_screen_key->virt_code == 0x2C, "PRINT_SCREEN virt code should be 0x2C");

KEY_DEF *scrolllock_key = find_key_def_by_name("SCROLLLOCK");
EXPECT(scrolllock_key != NULL, "SCROLLLOCK key should be found");
EXPECT(scrolllock_key->scan_code == 0x46, "SCROLLLOCK scan code should be 0x46");
EXPECT(scrolllock_key->virt_code == 0x91, "SCROLLLOCK virt code should be 0x91");

KEY_DEF *pause_key = find_key_def_by_name("PAUSE");
EXPECT(pause_key != NULL, "PAUSE key should be found");
EXPECT(pause_key->scan_code == 0xE11D, "PAUSE scan code should be 0xE11D");
EXPECT(pause_key->virt_code == 0x13, "PAUSE virt code should be 0x13");


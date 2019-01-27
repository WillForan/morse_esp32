// ascii <-> hid
int SHIFT_FLAG = 0x80;
// https://circuitpython.readthedocs.io/projects/hid/en/latest/_modules/adafruit_hid/keyboard_layout_us.html
int ASCII_TO_KEYCODE[] = {
        0x00,    // NUL
        0x00,    // SOH
        0x00,    // STX
        0x00,    // ETX
        0x00,    // EOT
        0x00,    // ENQ
        0x00,    // ACK
        0x00,    // BEL \a
        0x2a,    // BS BACKSPACE \b (called DELETE in the usb.org document)
        0x2b,    // TAB \t
        0x28,    // LF \n (called Return or ENTER in the usb.org document)
        0x00,    // VT \v
        0x00,    // FF \f
        0x00,    // CR \r
        0x00,    // SO
        0x00,    // SI
        0x00,    // DLE
        0x00,    // DC1
        0x00,    // DC2
        0x00,    // DC3
        0x00,    // DC4
        0x00,    // NAK
        0x00,    // SYN
        0x00,    // ETB
        0x00,    // CAN
        0x00,    // EM
        0x00,    // SUB
        0x29,    // ESC
        0x00,    // FS
        0x00,    // GS
        0x00,    // RS
        0x00,    // US
        0x2c,    // SPACE
        0x9e,    // ! x1e|SHIFT_FLAG (shift 1)
        0xb4,    // " x34|SHIFT_FLAG (shift ')
        0xa0,    // # x20|SHIFT_FLAG (shift 3)
        0xa1,    // $ x21|SHIFT_FLAG (shift 4)
        0xa2,    // % x22|SHIFT_FLAG (shift 5)
        0xa4,    // & x24|SHIFT_FLAG (shift 7)
        0x34,    // '
        0xa6,    // ( x26|SHIFT_FLAG (shift 9)
        0xa7,    // ) x27|SHIFT_FLAG (shift 0)
        0xa5,    // * x25|SHIFT_FLAG (shift 8)
        0xae,    // + x2e|SHIFT_FLAG (shift =)
        0x36,    // ,
        0x2d,    // -
        0x37,    // .
        0x38,    // /
        0x27,    // 0
        0x1e,    // 1
        0x1f,    // 2
        0x20,    // 3
        0x21,    // 4
        0x22,    // 5
        0x23,    // 6
        0x24,    // 7
        0x25,    // 8
        0x26,    // 9
        0xb3,    // : x33|SHIFT_FLAG (shift ;)
        0x33,    // ;
        0xb6,    // < x36|SHIFT_FLAG (shift ,)
        0x2e,    // =
        0xb7,    // > x37|SHIFT_FLAG (shift .)
        0xb8,    // ? x38|SHIFT_FLAG (shift /)
        0x9f,    // @ x1f|SHIFT_FLAG (shift 2)
        0x84,    // A x04|SHIFT_FLAG (shift a)
        0x85,    // B x05|SHIFT_FLAG (etc.)
        0x86,    // C x06|SHIFT_FLAG
        0x87,    // D x07|SHIFT_FLAG
        0x88,    // E x08|SHIFT_FLAG
        0x89,    // F x09|SHIFT_FLAG
        0x8a,    // G x0a|SHIFT_FLAG
        0x8b,    // H x0b|SHIFT_FLAG
        0x8c,    // I x0c|SHIFT_FLAG
        0x8d,    // J x0d|SHIFT_FLAG
        0x8e,    // K x0e|SHIFT_FLAG
        0x8f,    // L x0f|SHIFT_FLAG
        0x90,    // M x10|SHIFT_FLAG
        0x91,    // N x11|SHIFT_FLAG
        0x92,    // O x12|SHIFT_FLAG
        0x93,    // P x13|SHIFT_FLAG
        0x94,    // Q x14|SHIFT_FLAG
        0x95,    // R x15|SHIFT_FLAG
        0x96,    // S x16|SHIFT_FLAG
        0x97,    // T x17|SHIFT_FLAG
        0x98,    // U x18|SHIFT_FLAG
        0x99,    // V x19|SHIFT_FLAG
        0x9a,    // W x1a|SHIFT_FLAG
        0x9b,    // X x1b|SHIFT_FLAG
        0x9c,    // Y x1c|SHIFT_FLAG
        0x9d,    // Z x1d|SHIFT_FLAG
        0x2f,    // [
        0x31,    // \ backslash
        0x30,    // ]
        0xa3,    // ^ x23|SHIFT_FLAG (shift 6)
        0xad,    // _ x2d|SHIFT_FLAG (shift -)
        0x35,    // `
        0x04,    // a
        0x05,    // b
        0x06,    // c
        0x07,    // d
        0x08,    // e
        0x09,    // f
        0x0a,    // g
        0x0b,    // h
        0x0c,    // i
        0x0d,    // j
        0x0e,    // k
        0x0f,    // l
        0x10,    // m
        0x11,    // n
        0x12,    // o
        0x13,    // p
        0x14,    // q
        0x15,    // r
        0x16,    // s
        0x17,    // t
        0x18,    // u
        0x19,    // v
        0x1a,    // w
        0x1b,    // x
        0x1c,    // y
        0x1d,    // z
        0xaf,    // { x2f|SHIFT_FLAG (shift [)
        0xb1,    // | x31|SHIFT_FLAG (shift \)
        0xb0,    // } x30|SHIFT_FLAG (shift ])
        0xb5,    // ~ x35|SHIFT_FLAG (shift `)
        0x4c    // DEL DELETE (called Forward Delete in usb.org document)
};

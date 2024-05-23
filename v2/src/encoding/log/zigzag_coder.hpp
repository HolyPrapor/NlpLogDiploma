//
// Created by zeliboba on 5/23/24.
//

#ifndef DIPLOMA_ZIGZAG_CODER_HPP
#define DIPLOMA_ZIGZAG_CODER_HPP

class ZigzagCoder {
public:
    static int Encode(int value) {
        return (value << 1) ^ (value >> 31);
    }

    static int Decode(int value) {
        return (value >> 1) ^ -(value & 1);
    }
};

#endif //DIPLOMA_ZIGZAG_CODER_HPP

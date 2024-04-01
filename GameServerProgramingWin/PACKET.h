#pragma once

class BASIC_PACKET
{
public:
    short size{};
    char type{};
    int id{};

    BASIC_PACKET() {}
    BASIC_PACKET(int t) : type{ static_cast<char>(t) } {}
};

class KEY_PACKET : public BASIC_PACKET
{
public:
    int key{};

    KEY_PACKET(int t) : BASIC_PACKET(t) {
        key = 0;
        size = sizeof(KEY_PACKET);
    }
};

class POS_PACKET : public BASIC_PACKET
{
public:
    int x{}, y{};

    POS_PACKET(int t) : BASIC_PACKET(t) {
        x = y = 0;
        size = sizeof(POS_PACKET);
    }
};
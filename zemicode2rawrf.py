#!/usr/bin/python
# -*- coding: UTF-8 -*-

def zemicode2rawrf(zemicode, bit_one_is_long, usb_dongle):
    ''' Converts Zemismart RF 433 code to RfRaw sequence

    Args:
        zemicode (str): Zemismart RF 433 code
        bit_one_is_long (bool): True - 1 is long, 0 is short,
                                False - 1 is short, 0 is long
        usb_dongle (bool): True - USB Dongle signal format,
                           False - Remote Control signal format

    Returns:
        str: RfRaw sequence
    '''   
    
    result = ''
    if usb_dongle:
        # start pulse
        result += '2040, -4100'
    else:
        # start pulse
        result += '2121, -4250'

    bytes = zemicode.split(' ')
    for jj, hex_byte in enumerate(bytes):
        byte = int(hex_byte, 16)
        for ii in range(8):
            if bit_one_is_long == (byte & 0b10000000 == 0b10000000):
                # long pulse
                if usb_dongle:
                    result += ', 620, -1030'
                else:
                    result += ', 633, -962' if jj == 8 and ii == 7 else ', 633, -1063'
            else:
                # short pulse
                if usb_dongle:
                    result += ', 410, -410'
                else:
                    result += ', 422, -323' if jj == 8 and ii == 7 else ', 422, -427'
            byte <<= 1

    if usb_dongle:
        # stop pulse
        result += ', 252, -3120'
    else:
        # stop pulse
        result += ', 524, -6450'

    return result


def print_rawrf_codes(zemicode, bit_one_is_long, usb_dongle, description):
    ''' Prints result of convertion Zemismart RF 433 code to RfRaw sequence

    Args:
        zemicode (str): Zemismart RF 433 code
        bit_one_is_long (bool): True - 1 is long, 0 is short,
                                False - 1 is short, 0 is long
        usb_dongle (bool): True - USB Dongle signal format,
                           False - Remote Control signal format
        description (str): Description of Zemismart RF 433 code

    Returns:
        str: RfRaw sequence
    '''   

    print('==============================================================================================')
    print(zemicode + ' -- ' + ('USB Dongle' if usb_dongle else 'Remote Control'))
    print(description)
    print('----------------------------------------------------------------------------------------------')
    print(zemicode2rawrf(zemicode, bit_one_is_long, usb_dongle))
    print('----------------------------------------------------------------------------------------------')


if __name__ == "__main__":
    print_rawrf_codes('2F CB 9C 3E 2E BA 5D 3F 9F', False, True, 'UP')
    print_rawrf_codes('08 E9 BE 1C 0C 98 7F 1D B8', False, True, 'STOP')
    print_rawrf_codes('1F F8 AF 0D 1D 89 6E 0C AA', False, True, 'DOWN')

    print_rawrf_codes('B4 51 B9 7E B5 44 C6 7E BA', False, False, 'Remote Control 1 - UP')
    print_rawrf_codes('1D D9 30 F7 3D CC 4F F7 7B', False, False, 'Remote Control 1 - STOP')
    print_rawrf_codes('28 C8 26 E6 2C DD 59 E6 BC', False, False, 'Remote Control 1 - DOWN')

    






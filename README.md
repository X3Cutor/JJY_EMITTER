# ビルドの方法

README.rstをみてください

あと、接続先APの設定は src/main.cpp の中を見てください

# 使い方

デフォルトではGPIO 22番に40kHzのタイムコードが、GPIO 23番に60kHzのタイムコードが出ます。
以下のように抵抗を介して混合し、電線を電波時計のまわりにぐるぐる巻きにして GND で終端してください。

                   1k 〜 10k ぐらいの抵抗x2
    GPIO_23  -----v^v^v^v^------+
                                +----------➿➿➿➿➿➿--------> GND
    GPIO_22  -----v^v^v^v^------+           電波時計の周りにくるくる電線を巻く


# How to build

See README.rst

Also, see src/main.cpp for the settings of the AP to connect to

# How to use

By default, 40kHz time code is output to GPIO 22, and 60kHz time code is output to GPIO 23.
Mix them through a resistor as shown below, wrap the wire around the radio clock, and terminate with GND.

2 resistors of about 1k to 10k
    GPIO_23  -----v^v^v^v^------+
                                +----------➿➿➿➿➿➿--------> GND
    GPIO_22  -----v^v^v^v^------+ Wrap the wire around the radio clock
# navicat-keygen for linux - 如何使用?

1. 从官网下载Navicat，你应该会得到一个AppImage文件，例如 `navicat16-premium-en.AppImage`。

   我假定这个AppImage文件在 `~/` 文件夹下。

2. 提取 `navicat16-premium-en.AppImage` 里的所有文件到一个文件夹，例如 `~/navicat16-premium-en-patched`:

   ```bash
   $ mkdir ~/navicat16-premium-en
   $ sudo mount -o loop ~/navicat16-premium-en.AppImage ~/navicat16-premium-en
   $ cp -r ~/navicat16-premium-en ~/navicat16-premium-en-patched
   $ sudo umount ~/navicat16-premium-en
   $ rm -rf ~/navicat16-premium-en
   ```

3. [编译patcher和keygen](how-to-build.zh-CN.md)。

4. 使用 `navicat-patcher` 替换官方公钥。

   ```
   Usage:
     navicat-patcher [OPTION...] <navicat root path> [RSA-2048 private key file]
   
         --dry-run  Run patcher without applying any patches
     -h, --help     Print help
   ```

   __例如：__

   ```bash
   $ ./navicat-patcher ~/navicat16-premium-en-patched
   ```

   __Navicat Premium 16.0.7 英文版 for Linux__ 已经通过测试，下面是一份样例输出：

   ```
   ***************************************************
   *       navicat-patcher by @DoubleLabyrinth       *
   *               version: 16.0.7.0                 *
   ***************************************************
   
   [+] Try to open libcc.dll ... OK!
   
   [*] patch_solution_since<16, 0, 7, 0>: m_va_CSRegistrationInfoFetcher_LINUX_vtable = 0x0000000002fbf038
   [*] patch_solution_since<16, 0, 7, 0>: m_va_CSRegistrationInfoFetcher_LINUX_GenerateRegistrationKey = 0x0000000001420530
   [*] patch_solution_since<16, 0, 7, 0>: m_va_pltgot_std_string_append = 0x000000000306a608
   [+] patch_solution_since<16, 0, 7, 0>: official encoded key is found.
   
   [*] Generating new RSA private key, it may take a long time...
   [*] Your RSA private key:
   -----BEGIN RSA PRIVATE KEY-----
   MIIEogIBAAKCAQEAnqeNFqE0n6k+Ys58R+IzIULlZ9oxasJns46vaVcnd9e9mTj4
   hf/ArQohP5bex0UD+NGCLfQWKohnQxy9IFjQxZ6wUJnKOaA1UfdRr0ck7LZz5YV2
   5CkF4VZ7UWGEp/LEQiJnIBAtp7Zq5PNviqI03PZv2MfZt1/6YstCvi8s0rmpAyTw
   V5pteDipsI3lTapysLLsL+kQuJ0Z64GGk5D7rM8UPBt/Wjpe/qb/OwJKJ3Vi65/Z
   RRLVt46euwdiW8ORmNt552zOdvQPgYLEP38dMCpoeqIwv7IFWWJHxt0JgOTmxZ5c
   hjMq9ns3wyyEFjvSGSpM6sGL5eR1TMDQt8PSIwIDAQABAoIBAAp+jL+VdDSnbj/8
   5o2escEeeqwu65vjNhbTdljicfLka18qPI4oh6cqh158bUoDD6syuIivn5O6qBHx
   YbU1CsI3p/P86Dp9lWlRka6lZxTdULc6581ZxVDNdqTAbZTqYv745ZdiWpLAZzuz
   uooSBqsjBezx8z3E9Hv6c/S+jBl4IcKx7vEgKbC47CAAnFZFGonRPBj88fXn7VmP
   KEa7EVIE8yNG/xO4H+hlpio8XsTfbXB4Tx/Mw4f0ZCifFFE9aC503usbTQ9HCRn/
   13VgBeFzC+uxluEvpm7r3efcI1vFLDHiKlAzO85vDhTOef3jZv29PTUwbkNCP8AM
   CHP9abECgYEAzKWmGRcMVhwKL2RIc0VW1mSW82VTnjuTe9wnonYOGHmxCxQBZpB2
   j8mjfPBNzg3pjhRryrmfA2V/1Rxk7/hnw9I+OnRJA2p+USOKVlXi5YA3CMtPcevS
   DJaawPIkvjnnApSQ0CNYYKCgB43l33GJQ+btvYQiB/pmPwCiPhdsdYkCgYEAxndh
   GVSRd9yHez9LggsTfS4uo1OlZReHrtyX933eEG4YtiwM90Mb/9ZCA6CY5/o8YpOp
   AhPu7he3f67cQ7dotjilFHWqi4f+53d8NhfweQhM0azA9zzXjKhsao9jPjtTo+DW
   BptWVNZOqv1v0Np4/BZ3rTKtP42vSR4/Ql7Mi0sCgYATzoiH7yIjh2047wTQG0rv
   TycJAaqZKvz4RPOVFsYAem63OsVz7tF60zI+mmd9ZP1Q4gsYwORyCLXZo3jlfO5W
   FpgtQin66ai2I7F077UZL1KkSEE1LnTTARSTThxeSO5h4o0th+460/EJKiOwf6Wg
   a85gxFQi34pb2KzbQ5scuQKBgGLHA9LBnm0Tm3Kh/AjLTnXdSGUNuqHn5iYHsLMD
   OEThJvd0UTe3dPYOQ2jew3uhtfAyIcng9egWccPg2cvyOvGGm9LlBW7QzvORKocZ
   vxveH62z1461/2oIYX1fxDsy99v2iU9cfMlYqGq+HKrMMa7117aiJEwfToCLx1xX
   JmKlAoGAI8aOpSjyQ/xVUS1niXmwbKPkOJP7jI3yQ6YpUEnho/wpFw2/OjuVdXw2
   tcYFSg1GjBvTigDeh385TsjDa862SYAslgqZBPN4jXP20xw1kRr9kAQq6bvadC+b
   WpwZ8C5vNxwX5ccbxz3WwJhHtZvIQt0J8hOf3+BgcedHxa+bcTQ=
   -----END RSA PRIVATE KEY-----
   
   [*] patch_solution_since<16, 0, 7, 0>: Patch has been done.
   [*] New RSA-2048 private key has been saved to
       /home/doublesine/RegPrivateKey.pem

   *******************************************************
   *           PATCH HAS BEEN DONE SUCCESSFULLY!         *
   *                  HAVE FUN AND ENJOY~                *
   *******************************************************
   ```

5. 将文件重新打包成AppImage：

   __例如：__

   ```bash
   $ wget 'https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage'
   $ chmod +x appimagetool-x86_64.AppImage
   $ ./appimagetool-x86_64.AppImage ~/navicat16-premium-en-patched ~/navicat16-premium-en-patched.AppImage
   ```

6. 运行刚生成的AppImage：

   ```bash
   $ chmod +x ~/navicat16-premium-en-patched.AppImage
   $ ~/navicat16-premium-en-patched.AppImage
   ```

7. 使用 `navicat-keygen` 来生成 __序列号__ 和 __激活码__。

   ```
   Usage:
       navicat-keygen <-bin|-text> [-adv] <RSA-2048 Private Key File>

           <-bin|-text>                  Specify "-bin" to generate "license_file" used by Navicat 11.
                                         Specify "-text" to generate base64-encoded activation code.
                                         This parameter must be specified.

           [-adv]                        Enable advance mode.
                                         This parameter is optional.

           <RSA-2048 Private Key File>   A path to an RSA-2048 private key file.
                                         This parameter must be specified.
   ```

   __例如：__

   ```bash
   $ ./navicat-keygen -text ./RegPrivateKey.pem
   ```

   你会被要求选择Navicat产品类别、Navicat语言版本和填写主版本号。之后一个随机生成的 __序列号__ 将会给出。

   ```
   ***************************************************
   *       navicat-keygen by @DoubleLabyrinth        *
   *                version: 16.0.7.0                *
   ***************************************************

   [*] Select Navicat product:
    0. DataModeler
    1. Premium
    2. MySQL
    3. PostgreSQL
    4. Oracle
    5. SQLServer
    6. SQLite
    7. MariaDB
    8. MongoDB
    9. ReportViewer

   (Input index)> 1

   [*] Select product language:
    0. English
    1. Simplified Chinese
    2. Traditional Chinese
    3. Japanese
    4. Polish
    5. Spanish
    6. French
    7. German
    8. Korean
    9. Russian
    10. Portuguese

   (Input index)> 0
   
   [*] Input major version number:
   (range: 11 ~ 16, default: 16)> 16
   
   [*] Serial number:
   NAVB-EZF4-7T7X-9MPG
   
   [*] Your name:
   ```

   你可以使用这个 __序列号__ 来暂时激活Navicat。

   之后你会被要求填写 __用户名__ 和 __组织名__。你可以随意填写，但别太长。

   ```
   [*] Your name: Double Sine
   [*] Your organization: PremiumSoft CyberTech Ltd.

   [*] Input request code in Base64: (Double press ENTER to end)
   ```

   之后你会被要求填写请求码。__注意不要关闭keygen。__

8. __断开网络__. 找到注册窗口，填写keygen给你的 __序列号__，然后点击 `激活`。

9. 通常在线激活会失败，所以在弹出的提示中选择 `手动激活`。

10. 复制 __请求码__ 到keygen，连按两次回车结束。

    ```
    [*] Input request code in Base64: (Double press ENTER to end)
    ds7CnjEnNL+8Rme9Q5iD+3t9Tfuq9W6FzVN/3UZwC5zzecmM9EwyHJuZSovKJNSBTzL6AiGyxliTuKPWmLqAdwiKGLuD+mSaZ0syk0jTakVbXmbAk9maFkTz8SK5jMwnQVM/WBZcI0z2Jg1GnOCZVClu/Lo3/WF+XncS+alc2gshG9dUaI44Cqfvp/u1/EYso5fX/bjeBXaFW1/zj+uuRjVv5l0gt7JsTh9byGVxSDTO4zI64Iz9+58QYCbI9zKM+3G9Gou0UlNKjDYw4gN5+4dpiWAjitVTcL3oQzvflgAXjGlT/P6MA+8Xb5PEPJrEdxsErJObxBhO4cTH52wKoQ==

    [*] Request Info:
    {"K":"NAVBEZF47T7X9MPG", "DI":"AFCFB038A240942D8776", "P":"linux"}

    [*] Response Info:
    {"K":"NAVBEZF47T7X9MPG","DI":"AFCFB038A240942D8776","N":"Double Sine","O":"PremiumSoft CyberTech Ltd.","T":1644837835}

    [*] Activation Code:
    OY8Ib0brsepeS99it4s4WTDPQuKgu93WembLJ0bzr6M30Wh24reH1/ocaZ2Ek1bRBi5lqu2xBv/MpAcFUlstJANtavArkFnXYv0ZZiF3VF70De5GMe/VjkreNhjCGtTZcQKr8fabBTPjJuN0P+Hi1xWwMs9zJMuH+MJTmCQpbM4gu86YrFK/EDcdHtA4ZFgUI0SgYW8lwFausLFHp7C4uIQNbjtv4KP3XolDUrAx4lqg6bklgZ9C8ZjUpg28VVR9Ym37b1Fup7Y7C8OjmmMiAp8N5z8m6cA/EjcSLfLOMGf8jsAK0GHz5/AGUqAXWifv9h9cxPA35UgytqI9F2IH/Q==
    ```

11. 最终你会得到一个base64编码的 __激活码__。

    将之复制到 `手动激活` 的窗口，然后点击 `激活`。

    如果没有什么意外，应该可以成功激活。

12. 最后清理：

    ```bash
    $ rm ~/navicat16-premium-en.AppImage
    $ rm -rf ~/navicat16-premium-en-patched
    $ mv ~/navicat16-premium-en-patched.AppImage ~/navicat16-premium-en.AppImage
    ```

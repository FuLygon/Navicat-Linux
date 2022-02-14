# navicat-keygen for linux - How to use?

[中文版](how-to-use.zh-CN.md)

1. Download navicat from official website and you will get an AppImage file. For example, a file named `navicat16-premium-en.AppImage`. I assume that the AppImage file is located in `~/` folder.

2. Extract all files inside `navicat16-premium-en.AppImage` to a directory, e.g. `~/navicat16-premium-en-patched`:

   ```bash
   $ mkdir ~/navicat16-premium-en
   $ sudo mount -o loop ~/navicat16-premium-en.AppImage ~/navicat16-premium-en
   $ cp -r ~/navicat16-premium-en ~/navicat16-premium-en-patched
   $ sudo umount ~/navicat16-premium-en
   $ rm -rf ~/navicat16-premium-en
   ```

3. [Build keygen and patcher.](how-to-build.md)

4. Use `navicat-patcher` to replace the official public key.

   ```
   Usage:
     navicat-patcher [OPTION...] <navicat root path> [RSA-2048 private key file]
   
         --dry-run  Run patcher without applying any patches
     -h, --help     Print help
   ```

   __Example:__

   ```bash
   $ ./navicat-patcher ~/navicat16-premium-en-patched
   ```

   It has been tested on __Navicat Premium 16.0.7 English For Linux__ version. The following is an example of output:

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

5. Repack files into a new AppImage:

   __Example:__

   ```bash
   $ wget 'https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage'
   $ chmod +x appimagetool-x86_64.AppImage
   $ ./appimagetool-x86_64.AppImage ~/navicat16-premium-en-patched ~/navicat16-premium-en-patched.AppImage
   ```

6. Run the newly-generated AppImage:

   ```bash
   $ chmod +x ~/navicat16-premium-en-patched.AppImage
   $ ~/navicat16-premium-en-patched.AppImage
   ```

7. Use `navicat-keygen` to generate __snKey__ and __Activation Code__.

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

   __Example:__

   ```bash
   $ ./navicat-keygen -text ./RegPrivateKey.pem
   ```

   You will be asked to select Navicat language and give major version number. After that an randomly generated __snKey__ will be given.

   ```
   **********************************************************
   *       Navicat Keygen (Linux) by @DoubleLabyrinth       *
   *                   Version: 1.0                         *
   **********************************************************

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
   (range: 0 ~ 15, default: 12)> 15

   [*] Serial number:
   NAVM-RTVJ-EO42-IODD

   [*] Your name:
   ```

   You can use this __snKey__ to activate your Navicat preliminarily.

   Then you will be asked to input `Your name` and `Your organization`. Just set them whatever you want, but not too long.

   ```console
   [*] Your name: DoubleLabyrinth
   [*] Your organization: DoubleLabyrinth

   [*] Input request code in Base64: (Double press ENTER to end)
   ```

   After that, you will be asked to input request code. Now __DO NOT CLOSE KEYGEN__.

8. __Disconnect your network__. Find and click `Registration`.

   Fill license key by __Serial number__ that the keygen gave and click `Activate`.

9. Generally online activation will fail and Navicat will ask you do `Manual Activation`, just choose it.

10. Copy your request code and paste it in the keygen. Input empty line to tell the keygen that your input ends.

    ```console
    [*] Input request code in Base64: (Double press ENTER to end)
    OaGPC3MNjJ/pINbajFzLRkrV2OaSXYLr2tNLDW0fIthPOJQFXr84OOroCY1XN8R2xl2j7epZ182PL6q+BRaSC6hnHev/cZwhq/4LFNcLu0T0D/QUhEEBJl4QzFr8TlFSYI1qhWGLIxkGZggA8vMLMb/sLHYn9QebBigvleP9dNCS4sO82bilFrKFUtq3ch8r7V3mbcbXJCfLhXgrHRvT2FV/s1BFuZzuWZUujxlp37U6Y2PFD8fQgsgBUwrxYbF0XxnXKbCmvtgh2yaB3w9YnQLoDiipKp7io1IxEFMYHCpjmfTGk4WU01mSbdi2OS/wm9pq2Y62xvwawsq1WQJoMg==

    [*] Request Info:
    {"K":"NAVMRTVJEO42IODD", "DI":"4A12F84C6A088104D23E", "P":"linux"}

    [*] Response Info:
    {"K":"NAVMRTVJEO42IODD","DI":"4A12F84C6A088104D23E","N":"DoubleLabyrinth","O":"DoubleLabyrinth","T":1575543648}

    [*] Activation Code:
    i45HIr7T1g69Cm9g3bN1DBpM/Zio8idBw3LOFGXFQjXj0nPfy9yRGuxaUBQkWXSOWa5EAv7S9Z1sljlkZP6cKdfDGYsBb/4N1W5Oj1qogzNtRo5LGwKe9Re3zPY3SO8RXACfpNaKjdjpoOQa9GjQ/igDVH8r1k+Oc7nEnRPZBm0w9aJIM9kS42lbjynVuOJMZIotZbk1NloCodNyRQw3vEEP7kq6bRZsQFp2qF/mr+hIPH8lo/WF3hh+2NivdrzmrKKhPnoqSgSsEttL9a6ueGOP7Io3j2lAFqb9hEj1uC3tPRpYcBpTZX7GAloAENSasFwMdBIdszifDrRW42wzXw==
    ```

11. Finally, you will get __Activation Code__ which looks like a Base64 string.

    Just copy it and paste it in Navicat `Manual Activation` window, then click `Activate`.

    If nothing wrong, activation should be done successfully.

12. Clean up:

    ```bash
    $ rm ~/navicat16-premium-en.AppImage
    $ rm -rf ~/navicat16-premium-en-patched
    $ mv ~/navicat16-premium-en-patched.AppImage ~/navicat16-premium-en.AppImage
    ```

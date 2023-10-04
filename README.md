# Navicat Installation & Patch

Support up to Navicat 15.

## 1. Prerequisties


Assuming everything was downloaded in downloads directory (Example: `~/Downloads/`).

   * [git](https://git-scm.com/downloads/)
   * sudo apt-get install libssl-dev
   * [Navicat 15](https://www.navicat.com/en/download/navicat-premium)
   * capstone
   * [keystone](https://github.com/keystone-engine/keystone)
   * rapidjson
   * [clone this repository](https://github.com/FuLygon/Navicat-Linux)

   ```bash
   # install git
   $ sudo apt install git

   # download navicat 15 from official website
   > https://www.navicat.com/en/download/navicat-premium

   # install capstone
   $ sudo apt install libcapstone-dev

   # install keystone
   $ sudo apt-get install cmake
   $ git clone https://github.com/keystone-engine/keystone.git
   $ cd keystone
   $ mkdir build
   $ cd build
   $ ../make-share.sh
   $ sudo make install
   $ sudo ldconfig

   # install rapidjson
   $ sudo apt-get install rapidjson-dev

   # clone this repository
   $ git clone https://github.com/FuLygon/Navicat-Linux.git
   ```

## 2. Installing Navicat

1. Mounting AppImage.
   ```bash
   $ mkdir ~/Downloads/navicat
   $ sudo mount -o loop ~/Downloads/navicat15-premium-en.AppImage ~/Downloads/navicat
   ```
2. Assuming i want to install navicat in /opt (Example: `/opt/navicat`).
   ```bash
   $ sudo cp -r ~/Downloads/navicat /opt/ && mv /opt/AppImage /opt/navicat
   ```
3. Unmount and clean up.
   ```bash
   $ sudo umount ~/Downloads/navicat/navicat
   $ rm -rf ~/Downloads/navicat/navicat
   ```

## 3. Patching Navicat

1. Build Navicat keygen & patcher.
   ```bash
   $ cd ~/Downloads/navicat-linux
   $ make all
   ```
   You will see executable files `navicat-patcher` and `navicat-keygen` in `bin/` directory.

2. Patching Navicat.  
   Use `navicat-patcher` to replace official public key.
   ```
   Usage:
      navicat-patcher [--dry-run] <Navicat Installation Path> [RSA-2048 Private Key File]

         [--dry-run]                   Run patcher without applying any patches.
                                       This parameter is optional.

         <Navicat Installation Path>   Path to a directory where Navicat locates
                                       This parameter must be specified.

         [RSA-2048 Private Key File]   Path to a PEM-format RSA-2048 private key file.
                                       This parameter is optional.
   ```
   __Example:__
   ```bash
   $ cd ~/Downloads/navicat-linux/bin
   $ ./navicat-patcher /opt/navicat
   ```
   __Output:__
   ```console
   **********************************************************
   *       Navicat Patcher (Linux) by @DoubleLabyrinth      *
   *                  Version: 1.0                          *
   **********************************************************

   Press ENTER to continue or Ctrl + C to abort.

   [+] Try to open libcc.so ... Ok!

   [+] PatchSolution0 ...... Ready to apply
      RefSegment      =  1
      MachineCodeRva  =  0x000000000136f2b0
      PatchMarkOffset = +0x000000000291b260

   [*] Generating new RSA private key, it may take a long time...
   [*] Your RSA private key:
      -----BEGIN RSA PRIVATE KEY-----
      MIIEpAIBAAKCAQEAtiq7+TZ+2RlgZZoy1DOtLLWOEZyEhUIOsZTBW6RMfZRemDHo
      pRjRQpdnNnR9g52XJhuLK5B1E4y8ltGhEgQyEcunXgHOnOS2wYTimX1mob91h6Tw
      bevLEayKNE8pQQjPGobzWcts0suV7Ap80z8DLXbncc/p4EdbzgH8C0iKzChNEvJr
      D9uRc+mDqFUJZgkJyLo/PLWwbfjHJ76LgCzN++7dV63wZ3phcCk4Ban2W1sj+Byt
      xg6IGf7L64o9MEQZNB0cnhu0zmrp2gwlR55DbXs+DtBsa31p1bz5DQQWpcay6cNZ
      qjj9icO30APHqFPRhG9HB13U7OSPtQF10eXwCQIDAQABAoIBAHweH9dMTx9CkBCK
      AKnez9ZPmY3cU9qryHEsRpZXGprRtaYDE10xzJqPqA5NTpuXiz22tJspIOwCDxi/
      mfw5iZfgZ5fAqXLyzqaKEQ69h2kbl0ulkP+EEedn5P6BFT2vb9TGMBZ1aR8zAWJD
      Pwt7uoGGq2k7XUGnvSBe78kMUy2H/ZlrvM9ICR6g7T7K6Gm5etRa0iSqpE0t+DW0
      LVPwmMbq2ipVLCtCpwuvS0iMwkDb9nzJa7lW7KaFt97SSC9BeDAskspWVUg+2/i9
      RL1sruKt+20RCbA1NHJ5gs2r+lyPzUgv+Z22+qLXGxIopDP6Lc9je/Gznn6Fa4v4
      EGHEeQECgYEA3q9SQfJtNVNizwc2c2Y0tTybZpJKtI1Ziw/STZYB4yxZIHGyQgxk
      YOkm9cJzvD0cZbzqNKoO4a+jmsCpbarMfCqONZmlkbPXtx8q6HIRbQQprDMVPDQP
      z2MCpFezvm/MJMjQOFid3awFoPsqCV+4HLa5a+YNWMeFGR06/6pkjCECgYEA0Wua
      qr70ze/Lat/rQCdvLBzdssRiY+HwZtXna44j8RGHdM25df85vY0oksSBIBYaVIgU
      GhibTcAjcPB63tHlq02mzcxMobXQo3ZSc7xvzyDXcVWNFwSlHPbF1et1XmTORJWo
      K8iQdvJ9+fmqF+w7/8iSY53udw3fmPvGLzDlpukCgYEAtDcSzQgS1assNfvSV0YJ
      tfrckQAnsUMLin09Z0jVMjJ71sPszYMV4VAPFalO3LJgPv3KVOCjtoXcU6tD8trd
      LkeKW1qDIg13sF1q7j8SVzWdPB0DPHutp9qrErFfZ+k2Pza9PDyLVhCXZZaeiIOM
      jsBaAzZk6spdrtt3Gd2eDcECgYACucjQkh2zlHxsTq/MLPc4dsO38PBq5mTvobca
      YkY7fAPsVEUmsQS6X9z4VBBuL3t2QDMqx8k1sNuFO8wNnGPF4APcx9zzed9WUVLv
      aLN32bCULA3jD89qWhfnGeJ0MRGyONRwhXLD6ZIUL9iz6QI66QxmuYaCDC8t2ZER
      yiwjCQKBgQCoFwYC/ODs53cHUkGv0PmY4tN8/s4YboHQ4bYbdm27DcHpKL/SUji6
      y4/0RptEmB91Ot+CBh8Wb0Z6lvMDZi/nu0JkhjfDSXvaPxUpEFPXcJ0RK6dOqmJR
      kuGzsujjXmPQO4I+tX0gm0EDq7Pe5YC0uPZc/N1qQYi31whFUnZajg==
      -----END RSA PRIVATE KEY-----
   [*] Your RSA public key:
      -----BEGIN PUBLIC KEY-----
      MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtiq7+TZ+2RlgZZoy1DOt
      LLWOEZyEhUIOsZTBW6RMfZRemDHopRjRQpdnNnR9g52XJhuLK5B1E4y8ltGhEgQy
      EcunXgHOnOS2wYTimX1mob91h6TwbevLEayKNE8pQQjPGobzWcts0suV7Ap80z8D
      LXbncc/p4EdbzgH8C0iKzChNEvJrD9uRc+mDqFUJZgkJyLo/PLWwbfjHJ76LgCzN
      ++7dV63wZ3phcCk4Ban2W1sj+Bytxg6IGf7L64o9MEQZNB0cnhu0zmrp2gwlR55D
      bXs+DtBsa31p1bz5DQQWpcay6cNZqjj9icO30APHqFPRhG9HB13U7OSPtQF10eXw
      CQIDAQAB
      -----END PUBLIC KEY-----

   *******************************************************
   *                   PatchSolution0                    *
   *******************************************************
   [*] Previous:
   +0x0000000000000070                          01 00 00 00 05 00 00 00          ........
   +0x0000000000000080  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x0000000000000090  00 00 00 00 00 00 00 00 60 b2 91 02 00 00 00 00  ........`.......
   +0x00000000000000a0  60 b2 91 02 00 00 00 00 00 10 00 00 00 00 00 00  `...............
   [*] After:
   +0x0000000000000070                          01 00 00 00 05 00 00 00          ........
   +0x0000000000000080  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x0000000000000090  00 00 00 00 00 00 00 00 f0 b3 91 02 00 00 00 00  ................
   +0x00000000000000a0  f0 b3 91 02 00 00 00 00 00 10 00 00 00 00 00 00  ................

   [*] Previous:
   +0x000000000291b260  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b270  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b280  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b290  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2a0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2b0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2c0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2d0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2e0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b2f0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b300  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b310  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b320  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b330  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b340  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b350  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b360  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b370  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b380  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b390  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b3a0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b3b0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b3c0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b3d0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   +0x000000000291b3e0  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................
   [*] After:
   +0x000000000291b260  ef be ad de 4d 49 49 42 49 6a 41 4e 42 67 6b 71  ....MIIBIjANBgkq
   +0x000000000291b270  68 6b 69 47 39 77 30 42 41 51 45 46 41 41 4f 43  hkiG9w0BAQEFAAOC
   +0x000000000291b280  41 51 38 41 4d 49 49 42 43 67 4b 43 41 51 45 41  AQ8AMIIBCgKCAQEA
   +0x000000000291b290  74 69 71 37 2b 54 5a 2b 32 52 6c 67 5a 5a 6f 79  tiq7+TZ+2RlgZZoy
   +0x000000000291b2a0  31 44 4f 74 4c 4c 57 4f 45 5a 79 45 68 55 49 4f  1DOtLLWOEZyEhUIO
   +0x000000000291b2b0  73 5a 54 42 57 36 52 4d 66 5a 52 65 6d 44 48 6f  sZTBW6RMfZRemDHo
   +0x000000000291b2c0  70 52 6a 52 51 70 64 6e 4e 6e 52 39 67 35 32 58  pRjRQpdnNnR9g52X
   +0x000000000291b2d0  4a 68 75 4c 4b 35 42 31 45 34 79 38 6c 74 47 68  JhuLK5B1E4y8ltGh
   +0x000000000291b2e0  45 67 51 79 45 63 75 6e 58 67 48 4f 6e 4f 53 32  EgQyEcunXgHOnOS2
   +0x000000000291b2f0  77 59 54 69 6d 58 31 6d 6f 62 39 31 68 36 54 77  wYTimX1mob91h6Tw
   +0x000000000291b300  62 65 76 4c 45 61 79 4b 4e 45 38 70 51 51 6a 50  bevLEayKNE8pQQjP
   +0x000000000291b310  47 6f 62 7a 57 63 74 73 30 73 75 56 37 41 70 38  GobzWcts0suV7Ap8
   +0x000000000291b320  30 7a 38 44 4c 58 62 6e 63 63 2f 70 34 45 64 62  0z8DLXbncc/p4Edb
   +0x000000000291b330  7a 67 48 38 43 30 69 4b 7a 43 68 4e 45 76 4a 72  zgH8C0iKzChNEvJr
   +0x000000000291b340  44 39 75 52 63 2b 6d 44 71 46 55 4a 5a 67 6b 4a  D9uRc+mDqFUJZgkJ
   +0x000000000291b350  79 4c 6f 2f 50 4c 57 77 62 66 6a 48 4a 37 36 4c  yLo/PLWwbfjHJ76L
   +0x000000000291b360  67 43 7a 4e 2b 2b 37 64 56 36 33 77 5a 33 70 68  gCzN++7dV63wZ3ph
   +0x000000000291b370  63 43 6b 34 42 61 6e 32 57 31 73 6a 2b 42 79 74  cCk4Ban2W1sj+Byt
   +0x000000000291b380  78 67 36 49 47 66 37 4c 36 34 6f 39 4d 45 51 5a  xg6IGf7L64o9MEQZ
   +0x000000000291b390  4e 42 30 63 6e 68 75 30 7a 6d 72 70 32 67 77 6c  NB0cnhu0zmrp2gwl
   +0x000000000291b3a0  52 35 35 44 62 58 73 2b 44 74 42 73 61 33 31 70  R55DbXs+DtBsa31p
   +0x000000000291b3b0  31 62 7a 35 44 51 51 57 70 63 61 79 36 63 4e 5a  1bz5DQQWpcay6cNZ
   +0x000000000291b3c0  71 6a 6a 39 69 63 4f 33 30 41 50 48 71 46 50 52  qjj9icO30APHqFPR
   +0x000000000291b3d0  68 47 39 48 42 31 33 55 37 4f 53 50 74 51 46 31  hG9HB13U7OSPtQF1
   +0x000000000291b3e0  30 65 58 77 43 51 49 44 41 51 41 42 ad de ef be  0eXwCQIDAQAB....

   [*] Previous:
   +0x000000000136f2b0  44 0f b6 24 18 48 8b 44 24 28 8b 50 f8 85 d2 79  D..$.H.D$(.P...y
   +0x000000000136f2c0  6f                                               o               
   [*] After:
   +0x000000000136f2b0  45 31 e4 48 8d 05 aa bf 5a 01 90 90 90 90 90 90  E1.H....Z.......
   +0x000000000136f2c0  90                                               .               

   [*] New RSA-2048 private key has been saved to
      /home/fulygon/Downloads/navicat-keygen-tools-linux/bin/RegPrivateKey.pem

   *******************************************************
   *           PATCH HAS BEEN DONE SUCCESSFULLY!         *
   *                  HAVE FUN AND ENJOY~                *
   *******************************************************
   ```
3. Open Navicat.  
   ```bash
   $ /opt/navicat/AppRun
   ```
   If you can't open navicat, Try  
   ```bash
   $ sudo chmod +x /opt/navicat/AppRun
   ```
   After Navicat was opened, choose `Register` (Or `Help` > `Registeration`).
4. Generate key.
   Use `navicat-keygen` to generate __snKey__ and __Activation Code__.
   ```
   Usage:
    navicat-keygen <--bin|--text> [--adv] <RSA-2048 Private Key File>

        <--bin|--text>    Specify "-bin" to generate "license_file" used by Navicat 11.
                          Specify "-text" to generate base64-encoded activation code.
                          This parameter must be specified.

        [--adv]                       Enable advance mode.
                                      This parameter is optional.

        <RSA-2048 Private Key File>   A path to an RSA-2048 private key file.
                                      This parameter must be specified.
   ```
   __Example:__
   ```bash
   $ cd ~/Downloads/navicat-linux/bin
   $ ./navicat-keygen --text ./RegPrivateKey.pem
   ```
   You will be asked to select Navicat language and give major version number. After that an randomly generated __snKey__ will be given.
   ```console
   fulygon@Y530:~/Downloads/navicat-linux/bin$ ./navicat-keygen --text RegPrivateKey.pem
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
   NAVI-SFTW-5XLE-EID7

   [*] Your name:
   ```
   You can use this snKey to activate your Navicat preliminarily.    
   Then you will be asked to input `Your name` and `Your organization`. Just set them whatever you want, but not too long.
   ```
   [*] Your name: FuLygon
   [*] Your organization: ORG

   [*] Input request code in Base64: (Double press ENTER to end)
   ```
   After that, you will be asked to input request code. Now __DO NOT CLOSE KEYGEN__.

5. __Disconnect your network__. Find and click `Registration`. 
   
   Fill license key by __Serial number__ that the keygen gave and click `Activate`.

6. Generally online activation will fail and Navicat will ask you do `Manual Activation`, just choose it.

7. Copy your request code and paste it in the keygen. Input empty line to tell the keygen that your input ends.

   ```console
   [*] Input request code in Base64: (Double press ENTER to end)
   Eclwgrv03orbQuO7x1U3gme+p0SDS6G9MQBNtjz9rqiZKY5OrG1en1JJMyNZKe725h5ofM8Q/F779qlf/F1ePC5Shb41qwoH38hP88mlrGXFyMea4Jg+X2SSjNJT/0YBkMdwYcENgV4nCFFwbO4UCBdptpKHoCTHnpZDZerll/P8egNeFd+Qpa8cKtD5mbJiL9zYFq2sv++dMWHpAL6ZQIODM2B9XAm2gEWc4Lwhs/t+SPzjUwbH2L3gcRpbWuUUdFeRWRGDM3WP+1nJPxTr9L14mAbjgdXI/gzPm1gNtEK3GWuaIdDXw4BDSfc5LJQyHDM8dL00eRhB4rOOPr92pA==

   [*] Request Info:
   {"K":"NAVISFTW5XLEEID7", "DI":"117F1FD10D8518C58304", "P":"linux"}

   [*] Response Info:
   {"K":"NAVISFTW5XLEEID7","DI":"117F1FD10D8518C58304","N":"FuLygon","O":"ORG","T":1604463449}

   [*] Activation Code:
   MA8K14dWu9moX3IOS11Lyrgx0zUQgVmwAr149hjAyvDA+5BmdAIvDA04xPO8GvJJOitQe6Io7pp0Q4PLoRxLKCOoORE639izTv9k4P2gSndERI31vLPLWxPcXj4SIEtYLKKpu7jxx5eS9fax3HNtaVyeFK4jWykBgBkUCCzZlI33xaau7mKHgpfEFtgzn0F9ilrWRrzNhIzvz/Q7IDVvLm/7BtTbfHCw2+HkIrQgeaKC/nJ9wFYPhs/A0Z6pSULVuZI/bt/HYB+i6i3nW6hY/ZywF29PnSm6IxKtTIjpkAcXthtR3P2/eMIcFiuTcjCXurGNgbUj7iQ/4draNiWdjA==
   ```

8. Finally, you will get __Activation Code__ which looks like a Base64 string. 

   Just copy it and paste it in Navicat `Manual Activation` window, then click `Activate`. 
   
   If nothing wrong, activation should be done successfully.

## 4. Create Navicat shortcut (Optional)
1. Add shortcut to menu.  
   Open `navicat.desktop` located in `/opt/navicat`, edit these line:
   ```
   Icon=/opt/navicat/navicat-icon.png
   Exec=/opt/navicat/AppRun
   ```
   Change Permission & Owner.
   ```bash
   $ cd /opt/navicat
   $ sudo chmod +x navicat.desktop
   $ sudo chown fulygon navicat.desktop
   ```
   Create Shortcut.
   ```bash
   $ sudo cp /opt/navicat/navicat.desktop /usr/share/applications
   ```
2. Create custom terminal command to open Navicat.  
   Add this line to your .bashrc file (default ubuntu bashrc file location is in `~/.bashrc`).
   ```bash
   alias navicat='/opt/navicat/AppRun'
   ```
   Then open terminal and type `navicat`. Navicat should open up.

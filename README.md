# Navicat Installation & Patch

Support up to Navicat 15.

# Installation methods

## 1. Docker

Make sure [Docker](https://docs.docker.com/engine/install) is installed and running.

### Setup the repository

Clone the repository.

```shell
git clone https://github.com/FuLygon/Navicat-Linux.git && cd Navicat-Linux
```

Copy Navicat `AppImage` inside this directory.

### Start the patching script

```shell
docker compose run --build --rm patcher
```

Proceed with the patching script.

### Keygen and Activating Navicat

The final step will be generating serial key and activating Navicat. **REMEMBER TO DISCONNECT INTERNET DURING THIS PROCESS**.

```shell
Starting Keygen... (REMEMBER TO DISCONNECT INTERNET)
Continue? [Y/n] y
```

Continue the final step as usual until you at `Input request code in Base64`.

```shell
[*] Input major version number:
(range: 0 ~ 15, default: 12)> 15

[*] Serial number:
XXXX-XXXX-XXXX-XXXX

[*] Your name: FuLygon
[*] Your organization: FGN

[*] Input request code in Base64: (Double press ENTER to end)
```

**Do not close the terminal**. Go into `./build` directory, there should be a patched `AppImage` of Navicat. Execute this `AppImage` to open Navicat. Beginning the activation process.

**ANOTHER REMINDER TO DISCONNECT INTERNET**

Enter the `Serial number` provided by the script to Navicat and **Activate**.

There should be a new prompt show up with **Manual Activation** option. If **Manual Activation** option not appear, double check if you have disconnected internet.

A new **Manual Activation** window with 2 textbox **Request code** and **Activation Code** will appear.

Copy the **Request code** to the patching script and **DOUBLE PRESS ENTER** to continue as mentioned by the `keygen`.

The `keygen` will generate an `Activation Code`. Copy this value to **Activate Code** textbox of Navicat and **Activate** to complete the Manual Activation process.

Your Navicat should be activated by now.

## 2. Manual

Manual method documentation is unmaintained and might be outdated, or incompatible with some distros.

See [docs](doc/README.md) or further detail

# Credits

[DoubleLabyrinth](https://github.com/DoubleLabyrinth)

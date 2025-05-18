To install libusb-1.0 on macOS, the easiest method is using Homebrew, a popular package manager for macOS. Here's how you can do it: 

Open the Terminal. 

If you don't already have Homebrew installed, you can install it by running: 
```
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Once Homebrew is installed, run the following command to install libusb: 
```
brew install libusb
```
This will download and install the latest stable version of libusb. 

Now copy the following files from /usr/local/Cellar/libusb/<version>/lib/ to here 
```
libusb-1.0.0.dylib 
libusb-1.0.dylib 
libusb-1.0.a 
```

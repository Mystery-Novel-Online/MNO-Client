# Installation instructions
1. Open your terminal and run the following commands in order.

For Ubuntu 20.04 and lower:
```
sudo apt-get install qt5-default libqt5multimedia5 libqt5multimedia5-plugins libqt5multimediawidgets5 gstreamer1.0-libav
```
For Ubuntu 21.10 and higher:
```
sudo apt-get install libqt5core5a libqt5concurrent5 libqt5multimedia5 libqt5multimedia5-plugins libqt5widgets5 libqt5x11extras5 gstreamer1.0-libav
```

2. Update dro-client and dro-client.sh permissions
```
chmod +x dro-client.sh
chmod +x dro-client
```
3. Launch
```
./dro-client.sh
```

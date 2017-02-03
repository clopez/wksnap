# wksnap

Takes snapshots of websites or local pages with WebKit and save them as PNG files


## Building and using wksnap.

### Building wksnap

* Install WebKit2GTK+ development headers:
```
sudo apt-get install libwebkit2gtk-4.0-dev
```

* Build it as follows:
```
gcc -o wksnap wksnap.c $(pkg-config --cflags --libs webkit2gtk-4.0)
# Or run the build.sh script
```

* Now copy the binary to /usr/local/bin (for example)


### Using wksnap

The usage is pretty straightforward:
```
wksnap [website-uri|file] path-to-destination.png
```

### Using wksnap headless

You can use xvfb-run for running it headless

* Install xvfb
```
apt-get install xvfb
```

* Run wksnap with it:
```
xvfb-run wksnap uri file
```

## TODO

List of things I would like to add

 * Add support for saving the webpages in more formats than PNG (PDF?)
 * Automatically wrap wksnap with xvfb-run without asking the user to do it.
 * Make configurable the default size of the snapshoots
 * Detect when a website couldn't load and return something better than an empty image.

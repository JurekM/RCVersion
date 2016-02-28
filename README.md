# RCVersion
This program modifies version in Windows resource source (.RC) file.
The intention is that you can use it in an automated build to set version from build properties.

Run with /? parameter for command line options.

Example command lines:

  RCVersion C:\Projects\RCVersion\RCVersion\RCVersion.rc /b:$(SCCREVISION)
  RCVersion C:\Projects\RCVersion\RCVersion\RCVersion.rc /b:$(SCCREVISION) /m:1 /n:3 /r:0

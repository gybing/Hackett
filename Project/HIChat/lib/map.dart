import 'package:amap_base/amap_base.dart';
import 'package:flutter/material.dart';

class MapWidget extends StatefulWidget {
  MapWidget({Key key}) : super(key: key);

  @override
  MapWidgetState createState() => MapWidgetState();
}

class MapWidgetState extends State<MapWidget> {
  AMapController _controller;
  MyLocationStyle _locationStyle = MyLocationStyle(
    anchorU: 0.5,
    anchorV: 0.5,
    radiusFillColor: const Color(0xff0000dc),
    strokeColor: const Color(0x640000b4),
    strokeWidth: 0,
    myLocationType: LOCATION_TYPE_FOLLOW,
    interval: 3000,
    showMyLocation: true,
    showsAccuracyRing: true,
    showsHeadingIndicator: false,
    locationDotBgColor: Colors.white,
    locationDotFillColor: Colors.blue,
    enablePulseAnnimation: true,
  );

  void _updateLocationStyle() async {
    if (await Permissions().requestPermission()) {
      _controller.setMyLocationStyle(_locationStyle);
    }
  }

  @override
  Widget build(BuildContext context) {
    return AMapView(
      onAMapViewCreated: (controller) {
        _controller = controller;
        _updateLocationStyle();
      },
      amapOptions: AMapOptions(
        logoPosition: LOGO_POSITION_BOTTOM_LEFT,
        zOrderOnTop: false,
        mapType: MAP_TYPE_NORMAL,
        scaleControlsEnabled: false,
        zoomControlsEnabled: false,
        compassEnabled: false,
        scrollGesturesEnabled: true,
        zoomGesturesEnabled: true,
        tiltGesturesEnabled: false,
        rotateGesturesEnabled: false,
      ),
    );
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }
}

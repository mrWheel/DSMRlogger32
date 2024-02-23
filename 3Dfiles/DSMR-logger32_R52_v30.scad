//---------------------------------------------------------
// Yet Another Parameterized Projectbox generator
//
//  This is a box for a DSMR-logger32 v5.2 PCB
//  for use with the YAPP_generator_v30 lbrary
//
//  Version 3.0 (03-01-2024)
//
// This design is parameterized based on the size of a PCB.
//---------------------------------------------------------


//-- these parms need to be declared before including the YAPPgenerator
//
//-- is the filament transparent (true) or not (false)
//-- if (true) lightTubes will go all the way through the Lid
transparentFilament = true;  //-- {false|true}

//-- select oled size (both 'false' for none) -----
oled_09_inch          = false;
oled_13_inch          = true;

makeBaseShell        = false;
makeLidShell         = false;
makeSwitchExtenders  = false;
makeOledStand        = true;


if (oled_09_inch && oled_13_inch)
{
  for (i = [0 : 1 : 20])
  {
    echo("====================================");
    echo("Select (only one!) OLED screen size");
    echo("====================================");
  }
}
//
//- these dimensions are for a 1.3" OLED display
//-- oled dimensions and Header position
//                              //               V-------- HeaderY
oledHeader13X       = 16;       //       +~~~~~~~~~~~~~~~+            
oledHeader13Y       = 40;       //       |     ==o==     | HeaderX    ^
oledPcb13Width      = 37;       //       +---------------+ ScreenXs   |
oledPcb13Height     = 24;       //       |               |            |
oledScreen13Xs      =  3;       //       |               |            > Height
oledScreen13Xe      = 26;       //       |_______________|            |
                                //       +---------------+ ScreenXe   |
                                //       |               |            |
                                //       +---------------+            v
                                //       <--- Width ----->


//-- these dimensions are for a 0.9" OLED display
//-- oled dimensions and Header position
//                              //               V-------- HeaderY
oledHeader09X       = 17;       //       +~~~~~~~~~~~~~~~+            
oledHeader09Y       = 40;       //       |     ==o==     | HeaderX    ^
oledPcb09Width      = 28;       //       +---------------+ ScreenXs   |
oledPcb09Height     = 19;       //       |               |            |
oledScreen09Xs      =  1.5;     //       |               |            > Height
oledScreen09Xe      = 19.0;     //       |_______________|            |
                                //       +---------------+ ScreenXe   |
                                //       |               |            |
                                //       +---------------+            v
                                //       <--- Width ----->



oledScreen09Width  = oledPcb09Width;
oledScreen09Height = oledScreen09Xe - oledScreen09Xs;
oledScreen09X      = oledHeader09X + oledScreen09Xs;
oledScreen09Y      = oledHeader09Y - (oledScreen09Width/2);

oledScreen13Width  = oledPcb13Width;
oledScreen13Height = oledScreen13Xe - oledScreen13Xs;
oledScreen13X      = oledHeader13X + oledScreen13Xs;
oledScreen13Y      = oledHeader13Y - (oledScreen13Width/2);

//-- normalize params -----
oledHeaderX       = (oled_13_inch) ? oledHeader13X : oledHeader09X;
oledHeaderY       = (oled_13_inch) ? oledHeader13Y : oledHeader09Y;
oledScreenXs      = (oled_13_inch) ? oledScreen13Xs : oledScreen09Xs;
oledScreenXe      = (oled_13_inch) ? oledScreen13Xe : oledScreen09Xe;
oledScreenWidth   = (oled_13_inch) ? oledScreen13Width : oledScreen09Width;
oledScreenHeight  = (oled_13_inch) ? oledScreen13Height : oledScreen09Height;

oledRecess = (oled_09_inch || oled_13_inch) 
                      ? [oledScreenXs+oledHeaderX, oledHeaderY-(oledScreenWidth/2)
                         , oledScreenHeight, oledScreenWidth
                         , 1, -30003] //-> yappRoundedRect
                      : [];

lensThickness = (transparentFilament) ? 0.5 : 0;


include <./YAPPgenerator_v3.scad>

//include <./roundedCubes.scad>


/*
see https://polyd.com/en/conversione-step-to-stl-online
*/

myPcb = "./STL/MODELS/DSMR-logger32_MODEL_R52.stl";

if (true)
{
  translate([-47.3,-157.5,5.0]) 
  {
    rotate([0,0,90]) color("white") import(myPcb);
  }
}


// Note: length/lengte refers to X axis, 
//       width/breedte to Y, 
//       height/hoogte to Z

/*
      padding-back|<------pcb length --->|<padding-front
                            RIGHT
        0    X-as ---> 
        +----------------------------------------+   ---
        |                                        |    ^
        |                                        |   padding-right 
        |                                        |    v
        |    -5,y +----------------------+       |   ---              
 B    Y |         | 0,y              x,y |       |     ^              F
 A    - |         |                      |       |     |              R
 C    a |         |                      |       |     | pcb width    O
 K    s |         |                      |       |     |              N
        |         | 0,0              x,0 |       |     v              T
      ^ |   -5,0  +----------------------+       |   ---
      | |                                        |    padding-left
      0 +----------------------------------------+   ---
        0    X-as --->
                          LEFT
*/

printBaseShell        = makeBaseShell;
printLidShell         = makeLidShell;
printSwitchExtenders  = makeSwitchExtenders; 
//printInsideOLED       = makeInsideOled;  
printOledStand        = makeOledStand; 

// Edit these parameters for your own board dimensions
wallThickness       = 2.2; 
basePlaneThickness  = 1.0;
lidPlaneThickness   = 1.2;
// Set the ratio between the wall thickness and the ridge height. 
//    Recommended to be left at 1.8 but for strong snaps.
wallToRidgeRatio = 1.5;

//-- total height = 14 + pcbThickness + standoffHeight
//--         19.6 = 14 + 1.6 + 4
baseWallHeight      = 13; 
lidWallHeight       = 11; //-- was 8;

// ridge where base and lid off box can overlap
// Make sure this isn't less than lidWallHeight
ridgeHeight         = 3.5;
ridgeSlack          = 0.2;
roundRadius         = 1.0;

// How much the PCB needs to be raised from the base
// to leave room for solderings and whatnot
standoffHeight      = 4.0;  //-- only used to position PCB
standoffDiameter    = 5;
standoffPinDiameter = 2.0;
standoffHoleSlack   = 0.4;


// Total height of box = basePlaneThickness + lidPlaneThickness 
//                     + baseWallHeight + lidWallHeight
pcbLength           = 77;
pcbWidth            = 80;
pcbThickness        = 1.7;
                            
// padding between pcb and inside wall
paddingFront        = 1;
paddingBack         = 1;
paddingRight        = 1;
paddingLeft         = 1;


//---------------------------
//--     C O N T R O L     --
//---------------------------
// -- Render --
renderQuality             = 6;          //-> from 1 to 32, Default = 8

// --Preview --
previewQuality            = 5;          //-> from 1 to 32, Default = 5
showSideBySide            = false;       //-> Default = true
onLidGap                  = 0;  // tip don't override to animate the lid opening
colorLid                  = "YellowGreen";   
alphaLid                  = 0.9;
colorBase                 = "BurlyWood";
alphaBase                 = 0.9;
hideLidWalls              = true;      //-> Remove the walls from the lid : only if preview and showSideBySide=true 
hideBaseWalls             = true;      //-> Remove the walls from the base : only if preview and showSideBySide=true  
showOrientation           = true;       //-> Show the Front/Back/Left/Right labels : only in preview
showPCB                   = false;      //-> Show the PCB in red : only in preview 
showSwitches              = false;      //-> Show the switches (for pushbuttons) : only in preview 
showButtonsDepressed      = false;      //-> Should the buttons in the Lid On view be in the pressed position
showOriginCoordBox        = false;      //-> Shows red bars representing the origin for yappCoordBox : only in preview 
showOriginCoordBoxInside  = false;      //-> Shows blue bars representing the origin for yappCoordBoxInside : only in preview 
showOriginCoordPCB        = false;      //-> Shows blue bars representing the origin for yappCoordBoxInside : only in preview 
showMarkersPCB            = false;      //-> Shows black bars corners of the PCB : only in preview 
showMarkersCenter         = false;      //-> Shows magenta bars along the centers of all faces  
inspectX                  = 0;          //-> 0=none (>0 from Back)
inspectY                  = 0;          //-> 0=none (>0 from Right)
inspectZ                  = 0;          //-> 0=none (>0 from Bottom)
inspectXfromBack          = true;       //-> View from the inspection cut foreward
inspectYfromLeft          = true;       //-> View from the inspection cut to the right
inspectZfromBottom        = true;       //-> View from the inspection cut up
//---------------------------
//--     C O N T R O L     --
//---------------------------


//===================================================================
// *** PCB Supports ***
// Pin and Socket standoffs 
//-------------------------------------------------------------------
//  Default origin =  yappCoordPCB : pcb[0,0,0]
//
//  Parameters:
//   Required:
//    p(0) = posx
//    p(1) = posy
//   Optional:
//    p(2) = Height to bottom of PCB : Default = standoffHeight
//    p(3) = PCB Gap : Default = -1 : Default for yappCoordPCB=pcbThickness, yappCoordBox=0
//    p(4) = standoffDiameter    Default = standoffDiameter;
//    p(5) = standoffPinDiameter Default = standoffPinDiameter;
//    p(6) = standoffHoleSlack   Default = standoffHoleSlack;
//    p(7) = filletRadius (0 = auto size)
//    n(a) = { <yappBoth> | yappLidOnly | yappBaseOnly }
//    n(b) = { <yappPin>, yappHole } // Baseplate support treatment
//    n(c) = { <yappAllCorners>, yappFrontLeft | yappFrontRight | yappBackLeft | yappBackRight }
//    n(d) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside }
//    n(e) = { yappNoFillet }
//-------------------------------------------------------------------
pcbStands = 
[
    [ 5, 4, standoffHeight, yappDefault, 6, yappBoth, yappFrontRight]
   ,[ 5, 5, standoffHeight, yappDefault, 8, yappBoth, yappBackLeft]
];     
            
//===================================================================
//  *** Connectors ***
//  Standoffs with hole through base and socket in lid for screw type connections.
//-------------------------------------------------------------------
//  Default origin =  yappCoordPCB : pcb[0,0,0]
//  
//  Parameters:
//   Required:
//    p(0) = posx
//    p(1) = posy
//    p(2) = StandHeight : From specified origin 
//    p(3) = screwDiameter
//    p(4) = screwHeadDiameter (don't forget to add extra for the fillet)
//    p(5) = insertDiameter
//    p(6) = outsideDiameter
//   Optional:
//    p(7) = insert Depth : default to entire connector
//    p(8) = PCB Gap : Default if yappCoordPCB then pcbThickness else 0
//    p(9) = filletRadius : Default = 0/Auto(0 = auto size)
//    n(a) = { yappAllCorners, yappFrontLeft | <yappBackLeft> | yappFrontRight | yappBackRight }
//    n(b) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside }
//    n(c) = { yappNoFillet }
//    n(d) = { yappCountersink }
//    n(e) = [yappPCBName, "XXX"] : Specify a PCB. Defaults to [yappPCBName, "Main"]
//-------------------------------------------------------------------
connectors   =  
[
//-- 0, 1, 2, 3,   4,   5,   6, 7,           8,           9
   [ 5, 5, 2, 2.9, 5.2, 4.1, 4, yappDefault, yappDefault, yappDefault, yappFrontLeft, yappBackRight]
];


//===================================================================
//  *** Cutouts ***
//    There are 6 cutouts one for each surface:
//      cutoutsBase (Bottom), cutoutsLid (Top), cutoutsFront, cutoutsBack, cutoutsLeft, cutoutsRight
//-------------------------------------------------------------------
//  Default origin = yappCoordBox: box[0,0,0]
//
//                        Required                Not Used        Note
//----------------------+-----------------------+---------------+------------------------------------
//  yappRectangle       | width, length         | radius        |
//  yappCircle          | radius                | width, length |
//  yappRoundedRect     | width, length, radius |               |     
//  yappCircleWithFlats | width, radius         | length        | length=distance between flats
//  yappCircleWithKey   | width, length, radius |               | width = key width length=key depth
//  yappPolygon         | width, length         | radius        | yappPolygonDef object must be
//                      |                       |               | provided
//----------------------+-----------------------+---------------+------------------------------------
//
//  Parameters:
//   Required:
//    p(0) = from Back
//    p(1) = from Left
//    p(2) = width
//    p(3) = length
//    p(4) = radius
//    p(5) = shape : { yappRectangle | yappCircle | yappPolygon | yappRoundedRect 
//                     | yappCircleWithFlats | yappCircleWithKey }
//  Optional:
//    p(6) = depth : Default = 0/Auto : 0 = Auto (plane thickness)
//    p(7) = angle : Default = 0
//    n(a) = { yappPolygonDef } : Required if shape = yappPolygon specified -
//    n(b) = { yappMaskDef } : If a yappMaskDef object is added it will be used as a mask 
//                             for the cutout.
//    n(c) = { [yappMaskDef, hOffset, vOffset, rotation] } : If a list for a mask is added 
//                              it will be used as a mask for the cutout. With the Rotation 
//                              and offsets applied. This can be used to fine tune the mask
//                              placement within the opening.
//    n(d) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside }
//    n(e) = { <yappOrigin>, yappCenter }
//    n(f) = { <yappGlobalOrigin>, yappLeftOrigin } // Only affects Top(lid), Back and Right Faces
//-------------------------------------------------------------------
cutoutsLid =  
[
    oledRecess
];

//----------------------+-----------------------+---------------+------------------------------------
//
//  Parameters:
//   Required:
//    p(0) = from Back
//    p(1) = from Left
//    p(2) = width
//    p(3) = length
//    p(4) = radius
//    p(5) = shape : { yappRectangle | yappCircle | yappPolygon | yappRoundedRect 
//                     | yappCircleWithFlats | yappCircleWithKey }
//  Optional:
//    p(6) = depth : Default = 0/Auto : 0 = Auto (plane thickness)
//    p(7) = angle : Default = 0
//    n(a) = { yappPolygonDef } : Required if shape = yappPolygon specified -
//    n(b) = { yappMaskDef } : If a yappMaskDef object is added it will be used as a mask 
//                             for the cutout.
//    n(c) = { [yappMaskDef, hOffset, vOffset, rotation] } : If a list for a mask is added 
//                              it will be used as a mask for the cutout. With the Rotation 
//                              and offsets applied. This can be used to fine tune the mask
//                              placement within the opening.
//    n(d) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside }
//    n(e) = { <yappOrigin>, yappCenter }
//    n(f) = { <yappGlobalOrigin>, yappLeftOrigin } // Only affects Top(lid), Back and Right Faces
//-------------------------------------------------------------------
cutoutsBase =  
[
//--           0,          1,       2,  3, 4, 5
    [shellLength/2, shellWidth/2 , 35, 35, 2, yappRoundedRect, [maskHoneycomb, 0, 2.5, 0], yappCenter, yappCoordBox]
];

//===================================================================
//  *** Cutouts ***
//    There are 6 cutouts one for each surface:
//      cutoutsBase (Bottom), cutoutsLid (Top), cutoutsFront, cutoutsBack, cutoutsLeft, cutoutsRight
//-------------------------------------------------------------------
//  Default origin = yappCoordBox: box[0,0,0]
//
//                        Required                Not Used        Note
//----------------------+-----------------------+---------------+------------------------------------
//  yappRectangle       | width, length         | radius        |
//  yappCircle          | radius                | width, length |
//  yappRoundedRect     | width, length, radius |               |     
//  yappCircleWithFlats | width, radius         | length        | length=distance between flats
//  yappCircleWithKey   | width, length, radius |               | width = key width length=key depth
//  yappPolygon         | width, length         | radius        | yappPolygonDef object must be
//                      |                       |               | provided
//----------------------+-----------------------+---------------+------------------------------------
//
//  Parameters:
//   Required:
//    p(0) = from Back
//    p(1) = from Left
//    p(2) = width
//    p(3) = length
//    p(4) = radius
//    p(5) = shape : { yappRectangle | yappCircle | yappPolygon | yappRoundedRect 
//                     | yappCircleWithFlats | yappCircleWithKey }
//  Optional:
//    p(6) = depth : Default = 0/Auto : 0 = Auto (plane thickness)
//    p(7) = angle : Default = 0
//    n(a) = { yappPolygonDef } : Required if shape = yappPolygon specified -
//    n(b) = { yappMaskDef } : If a yappMaskDef object is added it will be used as a mask 
//                             for the cutout.
//    n(c) = { [yappMaskDef, hOffset, vOffset, rotation] } : If a list for a mask is added 
//                              it will be used as a mask for the cutout. With the Rotation 
//                              and offsets applied. This can be used to fine tune the mask
//                              placement within the opening.
//    n(d) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside }
//    n(e) = { <yappOrigin>, yappCenter }
//    n(f) = { <yappGlobalOrigin>, yappLeftOrigin } // Only affects Top(lid), Back and Right Faces
//-------------------------------------------------------------------
cutoutsFront =  
[
  [23.5,  5,   11, 13, 1, yappRoundedRect, yappCenter]    // PWR Jack
 ,[37.5, -0.5, 15, 17, 1, yappRoundedRect]  // RJ11
 ,[56.0, -0.5, 15, 17, 1, yappRoundedRect]  // RJ11
];



//===================================================================
//  *** Box Mounts ***
//    Mounting tabs on the outside of the box
//-------------------------------------------------------------------
//  Default origin = yappCoordBox: box[0,0,0]
//
//  Parameters:
//   Required:
//    p(0) = pos : position along the wall : [pos,offset] : vector for position and offset X.
//                    Position is to center of mounting screw in leftmost position in slot
//    p(1) = screwDiameter
//    p(2) = width of opening in addition to screw diameter 
//                    (0=Circular hole screwWidth = hole twice as wide as it is tall)
//    p(3) = height
//   Optional:
//    p(4) = filletRadius : Default = 0/Auto(0 = auto size)
//    n(a) = { yappLeft | yappRight | yappFront | yappBack } : one or more
//    n(b) = { yappNoFillet }
//    n(c) = { <yappBase>, yappLid }
//    n(d) = { yappCenter } : shifts Position to be in the center of the opening instead of 
//                            the left of the opening
//    n(e) = { <yappGlobalOrigin>, yappLeftOrigin } : Only affects Back and Right Faces
boxMounts   = 
[
  [shellLength/2, 3.5, 15, 3, yappRight, yappCenter]
 ,[shellLength/2, 3.5, 15, 3, yappLeft, yappCenter]
];
               
//===================================================================
//  *** Snap Joins ***
//-------------------------------------------------------------------
//  Default origin = yappCoordBox: box[0,0,0]
//
//  Parameters:
//   Required:
//    p(0) = posx | posy
//    p(1) = width
//    p(2) = { yappLeft | yappRight | yappFront | yappBack } : one or more
//   Optional:
//    n(a) = { <yappOrigin>, yappCenter }
//    n(b) = { yappSymmetric }
//    n(c) = { yappRectangle } == Make a diamond shape snap
//-------------------------------------------------------------------
snapJoins   =  
[
  [36, 3, yappFront, yappRectangle]
 ,[78, 4, yappFront, yappRectangle]
 ,[15, 4, yappLeft,  yappRectangle]
 ,[70, 4, yappRight, yappRectangle]
 ,[15, 4, yappBack,  yappRectangle]
];
               
//===================================================================
//  *** Light Tubes ***
//-------------------------------------------------------------------
//  Default origin = yappCoordPCB: PCB[0,0,0]
//
//  Parameters:
//   Required:
//    p(0) = posx
//    p(1) = posy
//    p(2) = tubeLength
//    p(3) = tubeWidth
//    p(4) = tubeWall
//    p(5) = gapAbovePcb
//    p(6) = { yappCircle | yappRectangle } : tubeType    
//   Optional:
//    p(7) = lensThickness (how much to leave on the top of the lid for the 
//           light to shine through 0 for open hole : Default = 0/Open
//    p(8) = Height to top of PCB : Default = standoffHeight+pcbThickness
//    p(9) = filletRadius : Default = 0/Auto 
//    n(a) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside } 
//    n(b) = { <yappGlobalOrigin>, yappLeftOrigin }
//    n(c) = { yappNoFillet }
//-------------------------------------------------------------------
lightTubes = 
[
 //-- 0,  1, 2, 3, 4, 5, 6,             7
     [4, 18, 6, 3, 1, 2, yappRectangle, lensThickness]  // NeoPixel 1
    ,[4, 28, 6, 3, 1, 2, yappRectangle, lensThickness]  // NeoPixel 2
    ,[4, 42, 6, 6, 1, 2, yappCircle,    lensThickness]  // NeoPixel 3
];     

//===================================================================
//  *** Push Buttons ***
//-------------------------------------------------------------------
//  Default origin = yappCoordPCB: PCB[0,0,0]
//
//  Parameters:
//   Required:
//    p(0) = posx
//    p(1) = posy
//    p(2) = capLength 
//    p(3) = capWidth 
//    p(4) = capRadius 
//    p(5) = capAboveLid
//    p(6) = switchHeight
//    p(7) = switchTravel
//    p(8) = poleDiameter
//   Optional:
//    p(9) = Height to top of PCB : Default = standoffHeight + pcbThickness
//    p(10) = { yappRectangle | yappCircle | yappPolygon | yappRoundedRect 
//                    | yappCircleWithFlats | yappCircleWithKey } : Shape, Default = yappRectangle
//    p(11) = angle : Default = 0
//    p(12) = filletRadius          : Default = 0/Auto 
//    p(13) = buttonWall            : Default = 2.0;
//    p(14) = buttonPlateThickness  : Default= 2.5;
//    p(15) = buttonSlack           : Default= 0.25;
//    n(a) = { <yappCoordPCB> | yappCoordBox | yappCoordBoxInside } 
//    n(b) = { <yappGlobalOrigin>,  yappLeftOrigin }
//    n(c) = { yappNoFillet }
//-------------------------------------------------------------------
pushButtons = 
[
//-- RESET button -----------------
  [14, 6          // [0,1] posX, posY
    ,0            // [2] Width
    ,0            // [3] Length
    ,3            // [4] Radius
    ,-0.5         // [5] Cap above Lid
    ,2.5          // [6] Switch Height
    ,0.6          // [7] Switch travel
    ,3.5          // [8] Pole Diameter
    ,yappDefault  // [9]  Height to top of PCB
    ,yappCircle   // [10] Shape
    ,yappDefault  // [11]
    ,yappDefault  // [12]
    ,2            // [13]
  ]
//-- FLASH button -----------------
  ,[41,6          // [0,1] posX, posY
    ,8            // [2] Width
    ,8            // [3] Length
    ,2            // [4] Radius
    ,1.0          // [5] Cap above Lid
    ,2.5          // [6] Switch Height
    ,0.6          // [7] Switch travel
    ,3.5          // [8] Pole Diameter
    ,yappDefault  // [9] Height to top of PCB
    ,yappRoundedRect // [10] Shape
  ]

];     
                            
//===================================================================
//  *** Labels ***
//-------------------------------------------------------------------
//  Default origin = yappCoordBox: box[0,0,0]
//
//  Parameters:
//   p(0) = posx
//   p(1) = posy/z
//   p(2) = rotation degrees CCW
//   p(3) = depth : positive values go into case (Remove) negative valies are raised (Add)
//   p(4) = { yappLeft | yappRight | yappFront | yappBack | yappLid | yappBaseyappLid } : plane
//   p(5) = font
//   p(6) = size
//   p(7) = "label text"
//  Optional:
//   p(8) = Expand : Default = 0 : mm to expand text by (making it bolder) 
//-------------------------------------------------------------------
labelsPlane =  
[
  [19,  2, 0, 1, yappFront, "Liberation Mono:style=bold", 3, "5 Volt" ]
 ,[46,  2, 0, 1, yappFront, "Liberation Mono:style=bold", 3, "P1" ]
 ,[65,  2, 0, 1, yappFront, "Liberation Mono:style=bold", 3, "S1" ]
];

               
//---------------------------------------------------------------------------
module insideOledScreen(topPCB)
{
  zVal          = (baseWallHeight+lidWallHeight)-(standoffHeight-pcbThickness)-2;
  //zeroX         = pcbX+oledHeaderX-2.2;
  //zeroY         = pcbY+oledHeaderY-2.2;
  zeroX         = wallThickness+paddingBack+oledHeaderX-2.2;
  zeroY         = wallThickness+paddingLeft+oledHeaderY-2.2;
  
  rimWidth      = 2;
  rimXin        = oledScreenXs;
  rimYin        = (oledScreenWidth/2)*-1;
  rimXout       = rimXin-(rimWidth/2);
  rimYout       = rimYin-(rimWidth/2);
  wallFromRim   = 4.5;
  wallThickness = 4;
  wallInW       = oledScreenWidth+wallFromRim;
  wallInH       = oledScreenHeight+8;
  wallXin       = 0; //oledScreenXs-(wallFromRim/2);
  wallYin       = wallInW/-2;
  wallOutW      = oledScreenWidth+wallThickness+wallFromRim;
  wallOutH      = oledScreenHeight+6;
  wallXout      = 0;
  wallYout      = wallOutW/-2;
  
  echo(zeroX=zeroX, zeroY=zeroY);
  echo(rimXin=rimXin, rimYin=rimYin, rimYout=rimXout, rimYout=rimYout);
  echo(wallXin=wallXin, wallYin=wallYin, wallXout=wallXout, wallYout=wallYout);
  
  translate([zeroX, zeroY, (topPCB-zVal)])
  {
    //-- reference center of oled Screen Header
    //color("black") cylinder(r=1, h=10, center=true);
    
    //-- rim
    difference()
    {
      translate([rimXout,rimYout,-3])
      {
        color("red") cube([oledScreenHeight+rimWidth,oledScreenWidth+rimWidth,5.5]);
      }
      //-19-translate([rimXin,rimYin,-2])
      translate([rimXin,rimYin,-3.5])
      {
        //-19-color("white") cube([oledScreenHeight, oledScreenWidth, 4]);
        color("white") cube([oledScreenHeight, oledScreenWidth, 7]);
      }
    }
    
    //-- side wall's
    difference()
    {
      //-19-translate([wallXout,wallYout,-4])
      translate([wallXout,wallYout,-7])
      {
        //-19-color("green") cube([wallOutH,wallOutW,6]);
        color("green") cube([wallOutH,wallOutW,9]);
      }
      //-19-translate([wallXin-1,wallYin,-4.5]) 
      translate([wallXin-1,wallYin,-7.5]) 
      {
        color("blue") cube([wallInH+1, wallInW, 10.5]);
      }
    }
    
  }
  
} //  insideOledScreen()



module hookBaseOutside()
{
} //  hookBaseOutside()


module hookBaseInside()
{
} //  hookBaseInside()


module hookLidOutside()
{
  
} //  hookLidOutside()


module hookLidInside()
{
  //-- strengthening between RJ12's --
  translate([shellLength-24, 56.0, 0])
    rotate([90, 90, 0])
      linear_extrude(1.5)
        polygon(points=[[0,0],[0,23],[12,23]]);
      
  topPCB = (baseWallHeight+lidWallHeight)-(standoffHeight+2);

  if (printLidShell && (oled_09_inch || oled_13_inch )) insideOledScreen(topPCB);
  
} //  hookLidInside()


//-- oled Stand -----------
if (printOledStand)
{
    standHeight = 11;
  
    if (oled_13_inch)
    {
      //-- oled Stand 1.3"
      translate([-15,125,0])
      {
        translate([1,0,0])      cube([2,36, 2]);
        translate([-1,5,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,0,0])     color("blue")   cube([3,8,2]);  // 
        translate([-2.5,2,0])   color("black")  cube([4,1.5,1.5]);  // centreer nop
        translate([-1,28,0])    color("green")  cube([standHeight,3,2]); // stand
        translate([-1,33-5,0])  color("blue")   cube([3,8,2]);  //
        translate([-2.5,32,0])  color("red")    cube([4,1.5,1.5]);  // centreer nop
        //translate([-3,3,0]) cube([1,30,1]);
      }
    }
    if (oled_09_inch)
    {
      //-- oled Stand 0.96"
      translate([-15,125,0])
      {
        translate([1,0,0])      cube([2,oledScreenWidth-2, 2]);
        translate([-1,0,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,0,0])     color("blue")   cube([3,8,2]);  // 
        translate([-2.5,2,0])   color("black")  cube([4,1.5,1.5]);  // centreer nop
        translate([-1,oledScreenWidth-2,0])     color("green")  cube([standHeight,3,2]); // stand
        translate([-1,oledScreenWidth-7,0])     color("blue")   cube([3,8,2]);  //
        translate([-2.5,oledScreenWidth-3,0])   color("red")    cube([4,1.5,1.5]);  // centreer nop
        //translate([-3,3,0]) cube([1,30,1]);
      }
      //translate([-3,3,0]) color("black") cube([1,23,1]);
    }
      
    
} // .. printOledStand?


//---- This is where the magic happens ----
YAPPgenerate();

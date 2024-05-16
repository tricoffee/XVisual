#ifndef Constants_H
#define Constants_H

#include<QString>

class ImageSources
{
    public:
	       static const QString Background1;
		   static const QString Background2;
		   static const QString Background3;
		   static const QString Background4;
		   static const QString TextPointer;
		   static const QString FloodFill;
		   static const QString LineColor;
		   static const QString BringFront;
		   static const QString SendBack;
		   static const QString Delete;
		   static const QString Bold;
		   static const QString Italic;
		   static const QString Underline;
		   static const QString LinePointer;
		   static const QString RunButton;
		   static const QString ExportButton;
		   static const QString LoadButton;
		   static const QString XVisualLogo;
};

enum DiagramState { Insert, Move, Release };
enum DiagramType { Item, Line, Text, None };
enum BackGroundType { BlueGrid, WhiteGrid, GrayGrid, NoGrid };

#endif //Constants_H
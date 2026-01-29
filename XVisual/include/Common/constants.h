#ifndef Constants_H
#define Constants_H

#include <QString>

namespace XVisual {

class ImageSources
{
    public:
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
		   static const QString CancelRunButton;
		   static const QString ExportButton;
		   static const QString LoadButton;
		   static const QString XVisualLogo;
		   static const QString XVisualAbout;
};

enum DiagramState { Insert, Move, Release };
enum DiagramType { Item, Line, Text, None };

} // namespace XVisual

#endif //Constants_H

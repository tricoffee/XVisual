#ifndef ColleagueType_H
#define ColleagueType_H

namespace XVisual {

enum class ColleagueType
{
	NullType = 0,
	ItemType = 1,
	HandleType = 2
};

ColleagueType Number2ColleagueType(int number);

} // namespace XVisual

#endif // ColleagueType_H

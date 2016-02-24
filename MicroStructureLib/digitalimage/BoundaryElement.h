#ifndef BOUNDARY_ELEMENT_H_
#define BOUNDARY_ELEMENT_H_
namespace digitalimage {
	enum ORIENTATION {TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK};
	class BoundaryElement {
	public:
		BoundaryElement();
		BoundaryElement(const BoundaryElement& copy);
		BoundaryElement(int x, int y, int z, ORIENTATION orientation);
		BoundaryElement& operator=(const BoundaryElement& rhs);
		bool operator<(const BoundaryElement& rhs) const;
		bool operator>(const BoundaryElement& rhs) const;
		~BoundaryElement();
		friend bool operator==(const BoundaryElement& rhs, const BoundaryElement& lhs);
		int get_x();
		int get_y();
		int get_z();
		ORIENTATION get_orientation();
	private:
		int m_x;
		int m_y;
		int m_z;
		ORIENTATION m_orientation;
	};
	
}
#endif
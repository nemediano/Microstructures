#include "BoundaryElement.h"
#include <cstdlib>
#include <cassert>

namespace digitalimage {
	
	BoundaryElement::BoundaryElement() : m_x(-1), m_y(-1), m_z(-1), m_orientation(TOP) {
	}

	BoundaryElement::BoundaryElement(const BoundaryElement& copy) {
		m_x = copy.m_x;
		m_y = copy.m_y;
		m_z = copy.m_z;
		m_orientation = copy.m_orientation;
	}

	BoundaryElement::BoundaryElement(int x, int y, int z, ORIENTATION orientation) {
		assert(x >= 0);
		assert(y >= 0);
		assert(z >= 0);
		
		m_x = x;
		m_y = y;
		m_z = z;

		m_orientation = orientation;
	}

	BoundaryElement& BoundaryElement::operator=(const BoundaryElement& rhs) {
		m_x = rhs.m_x;
		m_y = rhs.m_y;
		m_z = rhs.m_z;
		m_orientation = rhs.m_orientation;

		return *this;
	}

	bool BoundaryElement::operator<(const BoundaryElement& rhs) const {
		if (m_x != rhs.m_x) {
			return m_x < rhs.m_x;
		} else if (m_y != rhs.m_y) {
			return m_y < rhs.m_y;
		} else if (m_z != rhs.m_z) {
			return m_z < rhs.m_z;
		} else if (m_orientation != rhs.m_orientation) {
			return m_orientation < rhs.m_orientation;
		}

		return false;
	}

	bool BoundaryElement::operator>(const BoundaryElement& rhs) const {
		return rhs < *this;
	}

	BoundaryElement::~BoundaryElement() {
	}
	
	bool operator==(const BoundaryElement& rhs, const BoundaryElement& lhs) {
		return (lhs.m_x == rhs.m_x) && (lhs.m_y == rhs.m_y) && (lhs.m_z == rhs.m_z) && (lhs.m_orientation == rhs.m_orientation);
	}

	int BoundaryElement::get_x() {
		return m_x;
	}

	int BoundaryElement::get_y() {
		return m_y;
	}

	int BoundaryElement::get_z() {
		return m_z;
	}

	ORIENTATION BoundaryElement::get_orientation() {
		return m_orientation;
	}

}
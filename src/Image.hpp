// Fall 2018

#pragma once

#include <string>

typedef unsigned int uint;

/**
 * An image, consisting of a rectangle of floating-point elements.
 * Each pixel element consists of 3 components: Red, Blue, and Green.
 *
 * This class makes it easy to save the image as a PNG file.
 * Note that colours in the range [0.0, 1.0] are mapped to the integer
 * range [0, 255] when writing PNG files.
 */
class Image {
public:
	// Construct an empty image.
	Image();

	// Construct a black image at the given width/height.
	Image(uint width, uint height);

	// Copy an image.
	Image(const Image & other);

	~Image();

	// Copy the data from one image to another.
	Image & operator=(const Image & other);

	// Returns the width of the image.
	uint width() const;

	// Returns the height of the image.
	uint height() const;

    // Retrieve a particular component from the image.
	double operator()(uint x, uint y, uint i) const;

	// Retrieve a particular component from the image.
	double & operator()(uint x, uint y, uint i);

	// Save this image into the PNG file with name 'filename'.
	// Warning: If 'filename' already exists, it will be overwritten.
	bool savePng(const std::string & filename) const;

	const double * data() const;
	double * data();

private:
	uint m_width;
	uint m_height;
	double * m_data;

	static const uint m_colorComponents;
};

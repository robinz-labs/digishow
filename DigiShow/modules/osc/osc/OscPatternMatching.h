/*
 Written by John MacCallum, The Center for New Music and Audio Technologies,
 University of California, Berkeley.  Copyright (c) 2009, The Regents of
 the University of California (Regents).
 Permission to use, copy, modify, distribute, and distribute modified versions
 of this software and its documentation without fee and without a signed
 licensing agreement, is hereby granted, provided that the above copyright
 notice, this paragraph and the following two paragraphs appear in all copies,
 modifications, and distributions.

 IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
 OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#ifndef OSC_PATTERN_MATCHING_H_
#define OSC_PATTERN_MATCHING_H_

#include <osc/OscAPI.h>

/*! \class OSCPatternMatching
 * \brief Class which provides OSC messages pattern matching
 *
 *  This class provides a static method for OSC messages pattern matching
 */
class OSC_API OSCPatternMatching
{
public:

	enum Type {
		/**
		 * Return code for osc_match() that indicates that the entire address was successfully matched
		 */
		OSC_MATCH_ADDRESS_COMPLETE = 1,
		/**
		 * Return code for osc_match() that indicates that the entire pattern was successfully matched
		 */
		OSC_MATCH_PATTERN_COMPLETE = 2
	};

	/**
	 * Match a pattern against an address.  In the case of a partial match, pattern_offset
	 * and address_offset will contain the number of bytes into their respective strings
	 * where the match failed.
	 *
	 * @param pattern The pattern to match
	 * @param address The address to match
	 * @param pattern_offset The number of bytes into the pattern that were matched successfully
	 * @param address_offset The number of bytes into the address that were matched successfully
	 * @return 0 if the match failed altogether, or an or'd combination of OSC_MATCH_ADDRESS_COMPLETE and
	 * OSC_MATCH_PATTERN_COMPLETE.
	 */
	static int osc_match(const char *pattern, const char *address, int *pattern_offset, int *address_offset);
};

#endif // OSC_PATTERN_MATCHING_H_

/*
 *  Webpage.h
 *  earthify
 *
 *  Created by Jeffrey Crouse on 2/14/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#pragma once
#include <iostream>
#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <tidy.h>
#include <buffio.h>
#include <string>
#include <map>
#include <curl/curl.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/xmlsave.h"

using namespace std;
class Webpage {
public:
	
	Webpage(string url, bool wellFormed=false);
	~Webpage();

	// Gets a map of the content and href od all links within a given xpath expression
	map<string,string> getLinks(string exp);
	
	
	// Gets the Google Maps address string out of a Craigslist page 
	// or an empty string if it doesn't exist.
	string	getGoogleAddress();
	
	// Gets the latitude and longitude out of a Google geocoding result 
	float* getGoogleLatLng();
	
	// Get all images from a page
	vector<string> getImages(string exp);
	
	const char* getNodeAsString(string exp);
	
protected:
	xmlXPathObjectPtr xpath(string exp);
	static int writeData(char *data, size_t size, size_t nmemb, std::string *buffer);

	xmlDocPtr doc;
	xmlXPathContextPtr xpathCtx;
	string contents;
};


/*
 *  Webpage.cpp
 *  earthify
 *
 *  Created by Jeffrey Crouse on 2/14/11.
 *  Copyright 2011 Eyebeam. All rights reserved.
 *
 */

#include "Webpage.h"

// -------------------------------------------------------------
string Webpage::userAgent = "Mozilla/5.0";
string Webpage::cacheDirectory = ".c2kcache";

// -------------------------------------------------------------
Webpage::Webpage(string url, bool wellFormed, bool verbose)
{
	locale loc;
	const collate<char>& coll = use_facet<collate<char> >(loc);
	long myhash = coll.hash(url.data(),url.data()+url.length());
	char cachefile[255];
	sprintf(cachefile, "%s/%ld.cache", Webpage::cacheDirectory.c_str(), myhash);
	
	string line;
	ifstream myfile(cachefile);
	if (myfile.is_open())
	{
		if(verbose) cerr << "loading from cache: " << cachefile << endl;
		while ( myfile.good() )
		{
			getline(myfile,line);
			contents += line+"\n";
		}
		myfile.close();
	}
	else 
	{
		if(verbose) cerr << "downloading..." << endl;
		
		static char errorBuffer[CURL_ERROR_SIZE];
		CURL *curl = curl_easy_init();
		CURLcode result;
		if (!curl) {
			throw "Couldn't create CURL object.";
		}
		
		// Set the headers
		struct curl_slist *headers=NULL;
		char user_agent_header[255];
		sprintf(user_agent_header, "User-Agent: %s", Webpage::userAgent.c_str());
		headers = curl_slist_append(headers, user_agent_header);
		

		// TO DO:
		// Set timeout limit
		// put the whoel thing in a try block
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contents);
		result = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
		
		// Did we succeed?
		if (result != CURLE_OK)
		{
			throw std::runtime_error(errorBuffer);						
		}
		
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		char status_msg[255];
		sprintf(status_msg, "HTTP status code: %ld", http_code);
		if(verbose) cerr << status_msg << endl;
		if (http_code != 200 || result == CURLE_ABORTED_BY_CALLBACK)
		{
			throw std::runtime_error("HTTP error");	
		}
		
		if(!wellFormed)
		{
			tidy_me();
		}
		
		ofstream myfile;
		myfile.open(cachefile, ios::out);
		if(myfile.is_open())
		{
			if(verbose) cerr << "Writing cachefile: " << cachefile << endl;
			myfile << contents;
			myfile.close();
		}
		else
		{
			if(verbose) cerr << "ERROR:  Couldn't write to " << cachefile << endl;
		}
	}
	

	// get rid of doctype line.  It messes up the parser
	string prefix = "<!DOCTYPE";
	if(contents.compare(0, prefix.size(), prefix)==0)
	{
		size_t pos = contents.find(">");
		contents.erase(0, pos+1);
	}

	doc = xmlParseMemory(contents.c_str(), contents.length());
	if (doc == NULL) {
		throw "Error: unable to parse HTML";
	}
	
	xpathCtx = xmlXPathNewContext(doc);
	if(xpathCtx == NULL) {
		throw std::runtime_error("Error: unable to create new XPath context");
	}
}


// -------------------------------------------------------------
Webpage::~Webpage()
{
	xmlFreeDoc(doc);
	xmlXPathFreeContext(xpathCtx); 
}

// -------------------------------------------------------------
void Webpage::tidy_me()
{
	try {
		TidyDoc _tdoc = tidyCreate();
		//tidyOptSetBool(_tdoc, tidyOptGetIdForName("show-body-only"), (Bool)1);
		tidyOptSetBool(_tdoc, tidyOptGetIdForName("output-xhtml"), (Bool)1);
		tidyOptSetBool(_tdoc, tidyOptGetIdForName("quote-nbsp"), (Bool)0);
		tidyOptSetBool(_tdoc, tidyOptGetIdForName("show-warnings"), (Bool)0);
		tidyOptSetValue(_tdoc, tidyOptGetIdForName("char-encoding"), "utf8");
		//tidyOptSetBool(_tdoc, tidyOptGetIdForName("ascii-chars"), (Bool)1);
		//tidyOptSetBool(_tdoc, tidyOptGetIdForName("markup"), (Bool)1);
		//tidyOptSetValue(_tdoc, tidyOptGetIdForName("indent"), "yes");
		//tidyOptSetValue(_tdoc, tidyOptGetIdForName("newline"), "\n");
		tidyOptSetInt(_tdoc, tidyOptGetIdForName("wrap"), 5000);
		tidyParseString( _tdoc, contents.c_str() );
		
		TidyBuffer output = {0};
		tidySaveBuffer(_tdoc, &output);
		contents = string((char*)output.bp, (size_t)output.size);
	} catch (exception& e) {
		throw e.what();
	}		
}

// -------------------------------------------------------------
xmlXPathObjectPtr Webpage::xpath(string exp)
{	
	const xmlChar* xpathExpr = BAD_CAST exp.c_str();
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
	if(xpathObj == NULL)
	{
		xmlXPathFreeContext(xpathCtx);
		throw std::runtime_error("Error: unable to evaluate xpath expression");
	}
	//std::cout << "results: " << xpathObj->nodesetval->nodeNr << endl;
	return xpathObj;
}

// -------------------------------------------------------------
map<string,string> Webpage::getLinks(string exp)
{
	map<string,string> links;

	xmlXPathObjectPtr obj = xpath(exp);
	xmlNodeSetPtr nodeset = obj->nodesetval;
	
	for (int i=0; i < nodeset->nodeNr; i++)
	{
		xmlChar *href, *title;
		href = xmlGetProp(nodeset->nodeTab[i], (const xmlChar *)"href");
		title = xmlNodeListGetString(doc, nodeset->nodeTab[i]->xmlChildrenNode, 1);
		links[(const char*)title] = (const char*)href;
	}

	xmlXPathFreeObject(obj);
	return links;
}


// -------------------------------------------------------------
const char* Webpage::getNodeAsString(string exp)
{
	xmlXPathObjectPtr obj = xpath(exp);
	xmlNodeSetPtr nodeset = obj->nodesetval;
	if(nodeset && nodeset->nodeNr>0)
	{
		xmlBufferPtr buf = xmlBufferCreate();
		xmlSaveCtxtPtr savectx = xmlSaveToBuffer(buf, 0, XML_SAVE_FORMAT);
		xmlNodePtr node = nodeset->nodeTab[0];
		if (savectx)
		{
			xmlSaveTree(savectx, node);
			xmlSaveClose(savectx);
		}
		return (const char*)xmlBufferContent(buf);
	}
	else
	{	
		cout << "no node found" << endl;
		return "";
	}
}

// -------------------------------------------------------------
const char* Webpage::getNodeAttribute(string exp, string attrib)
{
	const xmlChar* contents;
	xmlXPathObjectPtr obj = xpath(exp);
	xmlNodeSetPtr nodeset = obj->nodesetval;
	
	if(nodeset && nodeset->nodeNr > 0)
	{
		contents = xmlGetProp(nodeset->nodeTab[0], (const xmlChar *)attrib.c_str());
	}
	xmlXPathFreeObject(obj);

	return (const char*)contents;
}

// -------------------------------------------------------------
const char* Webpage::getNodeContents(string exp)
{
	const xmlChar* contents;
	xmlXPathObjectPtr obj = xpath(exp);
	xmlNodeSetPtr nodeset = obj->nodesetval;
	
	if(nodeset && nodeset->nodeNr > 0)
	{
		contents =  xmlNodeListGetString(doc, nodeset->nodeTab[0]->children, 1);
	}
	xmlXPathFreeObject(obj);
	
	return (const char*)contents;
}

/*
// -------------------------------------------------------------
// Gets the latitude and longitude out of a Google geocoding result 
// Replaced by other methods
float* Webpage::getGoogleLatLng()
{
	xmlXPathObjectPtr obj = xpath("/GeocodeResponse/status");
	const char* status = (const char*)xmlNodeListGetString(doc, obj->nodesetval->nodeTab[0]->children, 1);
	
	if(strcmp(status, "OK")!=0) {
		return NULL;
	}
	
	float* latlng = new float[2];
	
	// TO DO:  There must be a better way to get at the lat and lng values.
	obj = xpath("/GeocodeResponse/result/geometry/location");
	
	xmlNodeSetPtr nodeset = obj->nodesetval;
	xmlNode* location = nodeset->nodeTab[0];
	
	xmlNode* cur = location->xmlChildrenNode;
	const xmlChar* lat;
	const xmlChar* lng;
	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"lat"))){
			lat = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		}
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"lng"))){
			lng = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
		}
		cur = cur->next;
	}
	
	latlng[0] = atof((const char*)lat);
	latlng[1] = atof((const char*)lng);
	
	xmlXPathFreeObject(obj);
	return latlng;
}
*/
// -------------------------------------------------------------
int Webpage::writeData(char *data, size_t size, size_t nmemb, std::string *buffer)
{
	int result = 0;
	if (buffer != NULL) {
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

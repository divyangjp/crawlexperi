#include <iostream>
#include <string>
#include <curl/curl.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>
#include <htmlcxx/html/Uri.h>
#include <boost/regex.hpp>
#include <regex>
#include <string.h>
#include <algorithm>

using namespace std;
using namespace htmlcxx;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main(void)
{
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    //curl_easy_setopt(curl, CURLOPT_URL, "http://curl.haxx.se/libcurl/");
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.firstpost.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    //cout << readBuffer << endl;
    HTML::ParserDom parser;
    tree<HTML::Node> dom = parser.parseTree(readBuffer);

    //Print whole DOM tree
    //cout << dom << endl;

    tree<HTML::Node>::iterator it = dom.begin();
    tree<HTML::Node>::iterator end = dom.end();

    string tagarr[] = {"script", "style"};
    //print all text
    for(; it != end; ++it)
    {
        //if(it->isTag() && (strcasecmp(it->tagName().c_str(), "script") == 0))
        int tagarr_count = sizeof(tagarr)/sizeof(string);
        string* pstr = std::find(tagarr, tagarr+tagarr_count, it->tagName());
        if(pstr != tagarr+tagarr_count)
        {
            if((++it)->isTag())
                --it;
            continue;
        }
    	if((!it->isTag()) && (!it->isComment()))
    	{
            if(it->text().find_first_not_of(" \t\n\v\f\r") == std::string::npos)
            {
                continue;
            }
    		cout << "||" << it->text() << "||" << endl;
    		cout << "--------------------------" << endl;
    	}
    }

    cout << "print all links" << endl;

    const boost::regex brexpress("https?:\/\/[^\s/$.?#].[^\s]*");
    it = dom.begin();
    end = dom.end();
    for (; it != end; ++it)
      {
      	if (it->tagName() == "a")
      	{
      		it->parseAttributes();
      		if(boost::regex_match(it->attribute("href").second, brexpress))
      			cout << it->attribute("href").second << endl;
      	}
      }

    //std::cout << readBuffer << std::endl;
  }
  return 0;
}

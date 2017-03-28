# Metadefender C++ REST library

This sample library is a C++ wrapper which interacts with a subset of REST API functions. These sample codes are written mainly for [Metadefender Core v4.x](https://www.opswat.com/metadefender-core) but should be compatible with v3.x with minimum modification.
It is a header only library, so there is no need for building it.

## C API vs REST API
* **scanLocalFile**: */file* with filepath header for scanning a local file
* **scanFile**: */file* for sending a file for scan via POST
* **fetchScanResultById**: */file/{data_id}* for retrieving results via ID
* **fetchScanResultByHash**: */hash/{md5|sha1|sha256 hash}* for retrieving results via hash
* **fetchSanitizedFileById**: */file/converted/{data_id}* for retrieving a sanitized file via ID
* **createSession**: */login* for creating a session
* **destroySession**: */logout* for destroying a session
* **getVersionInfo**: */version* for retrieving details about the product version
* **getLicenseInfo**: */admin/license* for retrieving details about the licensing status of the product
* **getEngineInfo**: */stat/engines* for retrieving information about the engines

#Prerequisites for using the library
* GCC 4.9+ or VS2013+

#Prerequisites for building unit tests for the library
* CMake 2.8+
* GCC 4.9+ or VS2013+
* Working internet connection for downloading Google's unit test framework

# How to build unit tests
You should run CMake on test/CMakeLists.txt in the test directory and use system dependent tools on the CMake output.

## On Linux
Given you are standing in the root directory:

    $ mkdir build
    $ cd build
    $ cmake ../test
    $ make
	
The executable test will be available under *build/test/test-mdrestclient*

# How to use

The REST related functions can be accessed via the **MDRest** class, so you should create one first. In order to create MDRest you should provide *session* type via template parameter.

## Session type
You can find two types of HTTP sessions: **MDPersistentHttpSession** and **MDSimpleHttpSession**. MDPersistentHttpSession creates a persistent connection towards the REST server and re-uses it for every request, while MDSimpleHttpSession creates a new connection for every request. We recommend using MDPersistentHttpSession if you want to make consecutive requests. You should provide what type of *client* you would like to use for the session via template parameter.

## Client type
This should be a class implementing **IMDHttpClient**. The client is used for sending requests to and receiving responses from the REST server. See **MDCurlHttpClient** for example.

## Curl client
When using the curl client you should call **Opswat::MDCurlHttpClient::curlHttpClientInit()** or **Opswat::MDCurlHttpClient::curlHttpClientInitAll()** in your program. For more information about initializing please refer to the [curl\_global\_init documentation](https://curl.haxx.se/libcurl/c/curl_global_init.html).

Linking to libcurl and having libcurl headers are necessary if you want to use the MDCurlHttpClient in your application.

## Using results of requests
Most of the functions found in MDRest return **MDResponse** as a result. By calling **getJSON()** on the response you can get the plain JSON answer received from the REST server in a string without any JSON parsing overhead.
By calling **parse()** a struct will be created and populated by parsing the JSON. The struct's type depends on which function was called.

## Using API keys
If you need to call functions which require API keys on REST side (like receiving license informations) you should get an API key first.
After you have received an API key (for example after creating one with **createSession()**) you should call **useSession()** with your key. By invoking useSession the provided API key will be included in every requests in the future.

# Example
	#include <iostream>
	#include <string>
	#include <chrono>
	#include <thread>

	#include "md_rest.h"
	#include "md_curl_http_client.h"
	#include "md_http_session.h"

	using SessionType = Opswat::MDPersistentHttpSession<Opswat::MDCurlHttpClient>;

	int main(int argc, char* argv[])
	{
		Opswat::MDCurlHttpClient::curlHttpClientInitAll();
		try
		{
			// Create an MDRest object using persistent connection with curl client
			Opswat::MDRest<SessionType> rest(std::make_unique<SessionType>("127.0.0.1", 8008));
			
			// Create session
			auto parsedLogin = rest.createSession("admin", "admin")->parse();
			std::cout << "Session ID after login: " << parsedLogin->session_id << std::endl;
			
			rest.useSession(parsedLogin->session_id);

			// Initate scan for a local file by giving the path for the file and a display name
			std::unique_ptr<Opswat::MDResponse<Opswat::MDFileScanId>> response = rest.scanLocalFile("C:\\test.txt", "test");
			std::unique_ptr<Opswat::MDFileScanId> parsedResponse = response->parse();
			std::cout << "Data ID: " << parsedResponse->dataId << std::endl;

			// Give some time for scanning
			const int waitTime = 10;
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));

			// Check periodically if the scan is still processing by using the raw JSON so we don't make a lot of parsing
			auto scanResult = rest.fetchScanResultById(parsedResponse->dataId);
			while (scanResult->getJSON().find("\"result\":\"Processing\"") != std::string::npos)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
				scanResult = rest.fetchScanResultById(parsedResponse->dataId);
			}

			// Print the display name of the file and the overall scan result
			auto parsedScanResult = scanResult->parse();
			std::cout << "Result for " << parsedScanResult->fileInfo->displayName << " is: " << parsedScanResult->scanResult->scanAllResultDesc << std::endl;

			// Get engine infos and print the engine name and type for all of them
			auto parsedEngines = rest.getEngineInfo()->parse();
			for (auto &engine : parsedEngines->engines)
			{
				std::cout << "Engine name and type: " << engine->engineName << " -> " << engine->engineTypeDescription << std::endl;
			}

			// Use the previously created session ID for functions which require api key
			auto parsedVersion = rest.getVersionInfo()->parse();
			std::cout << "Metadefender version: " << parsedVersion->version << "\nProduct ID: " << parsedVersion->productId << std::endl;

			// Destroy session when it is not needed anymore
			auto logout = rest.destroySession(parsedLogin->session_id);
		}
		catch (Opswat::MDException& e)
		{
			std::cout << e.what() << std::endl;
		}

		return 0;
	}


# Notes
* Please note that the initialization function for curl is **not thread safe**. Please check it's [documentation](https://curl.haxx.se/libcurl/c/curl_global_init.html)
* Please use MDRest in a **try-catch** block. When facing an error the library throws different types of exceptions. For example **MDConnectionException** for connection errors or **MDParsingException** when
the parsing was unsuccessful.
* Please find the licenses for third party libraries under licenses folder.
* Persistent session with the curl client is **not thread safe**. The same MDRest object should not be used from different threads.
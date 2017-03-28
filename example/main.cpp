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
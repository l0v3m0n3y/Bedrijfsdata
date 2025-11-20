#ifndef BEDRIJFSDATA_H
#define BEDRIJFSDATA_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Bedrijfsdata {
private:
    std::string api_base = "https://free.bedrijfsdata.nl/v1.1";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("free.bedrijfsdata.nl"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    Bedrijfsdata(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> check_phone(int phone,const std::string& country_code="") {
        std::map<std::string, std::string> params;
        if (phone > 0) params["phone"] = std::to_string(phone);
        if (!country_code.empty()) params["country_code"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(country_code)));
        return make_api_call("/phone" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_postcode(const std::string& postcode,const std::string& country_code="") {
        std::map<std::string, std::string> params;
        if (!postcode.empty()) params["postcode"] =  utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(postcode)));
        if (!country_code.empty()) params["country_code"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(country_code)));
        return make_api_call("/postcode" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_city(const std::string& city,const std::string& country_code="") {
        std::map<std::string, std::string> params;
        if (!city.empty()) params["city"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(city)));
        if (!country_code.empty()) params["country_code"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(country_code)));
        return make_api_call("/city" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_geocoding(const std::string&  q,const std::string& country_code="") {
        std::map<std::string, std::string> params;
        if (!q.empty()) params["q"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(q)));
        if (!country_code.empty()) params["country_code"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(country_code)));
        return make_api_call("/geocoding" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_email(const std::string& email) {
        std::map<std::string, std::string> params;
        if (!email.empty()) params["email"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(email)));
        return make_api_call("/email" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_link(const std::string& url) {
        std::map<std::string, std::string> params;
        if (!url.empty()) params["url"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(url)));
        return make_api_call("/url" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_webrank(const std::string& domain) {
        std::map<std::string, std::string> params;
        if (!domain.empty()) params["domain"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(domain)));
        return make_api_call("/webrank" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_currency(const std::string& currency) {
        std::map<std::string, std::string> params;
        if (!currency.empty()) params["currency"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(currency)));
        return make_api_call("/currency" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_iban(const std::string& iban) {
        std::map<std::string, std::string> params;
        if (!iban.empty()) params["iban"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(iban)));
        return make_api_call("/iban" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_bic(const std::string& bic) {
        std::map<std::string, std::string> params;
        if (!bic.empty()) params["bic"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(bic)));
        return make_api_call("/bic" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_lei(const std::string& lei) {
        std::map<std::string, std::string> params;
        if (!lei.empty()) params["lei"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(lei)));
        return make_api_call("/lei" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_dns(const std::string& domain) {
        std::map<std::string, std::string> params;
        if (!domain.empty()) params["domain"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(domain)));
        return make_api_call("/dns" + build_query_params(params),"GET");
    }

    pplx::task<json::value> my_ip() {
        return make_api_call("/ip","GET");
    }

    pplx::task<json::value> check_password(const std::string& password, int threshold=25) {
        std::map<std::string, std::string> params;
        if (!password.empty()) params["password"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(password)));
        if (threshold>0) params["threshold"] = std::to_string(threshold);
        return make_api_call("/password" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_vat(const std::string& vat) {
        std::map<std::string, std::string> params;
        if (!vat.empty()) params["vat"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(vat)));
        return make_api_call("/vat" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_tender(const std::string& q) {
        std::map<std::string, std::string> params;
        if (!q.empty()) params["q"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(q)));
        return make_api_call("/tender" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_kvk(const std::string& kvk) {
        std::map<std::string, std::string> params;
        if (!kvk.empty()) params["kvk"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(kvk)));
        return make_api_call("/kvk" + build_query_params(params),"GET");
    }

    pplx::task<json::value> check_bag(const std::string& postcode , int number, const std::string& suffix="") {
        std::map<std::string, std::string> params;
        if (!postcode.empty()) params["postcode"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(postcode)));
        if (number > 0) params["number"] = std::to_string(number);
        if (!suffix.empty()) params["suffix"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(suffix)));
        return make_api_call("/bag" + build_query_params(params),"GET");
    }
};
#endif

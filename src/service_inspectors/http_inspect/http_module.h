//--------------------------------------------------------------------------
// Copyright (C) 2014-2016 Cisco and/or its affiliates. All rights reserved.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------
// http_module.h author Tom Peters <thopeter@cisco.com>

#ifndef HTTP_MODULE_H
#define HTTP_MODULE_H

#include <string>
#include <bitset>

#include "framework/module.h"

#include "http_enum.h"

#define HTTP_NAME "http_inspect"
#define HTTP_HELP "HTTP inspector"

struct HttpParaList
{
public:
    long request_depth;
    long response_depth;
    bool unzip;
    bool normalize_utf = true;
    struct UriParam
    {
    public:
        UriParam();
        ~UriParam() { delete[] unicode_map; }

        bool percent_u = false;
        bool utf8 = true;
        bool utf8_bare_byte = false;
        int oversize_dir_length = 300;
        bool iis_unicode = false;
        std::string iis_unicode_map_file;
        int iis_unicode_code_page = 1252;
        uint8_t* unicode_map = nullptr;
        bool iis_double_decode = false;
        bool backslash_to_slash = false;
        bool plus_to_space = true;
        bool simplify_path = true;
        std::bitset<256> bad_characters;
        std::bitset<256> unreserved_char;
        HttpEnums::CharAction uri_char[256];
    };
    UriParam uri_param;
#ifdef REG_TEST
    bool test_input;
    bool test_output;
    long print_amount;
    bool print_hex;
    bool show_pegs;
#endif
};

class HttpModule : public Module
{
public:
    HttpModule() : Module(HTTP_NAME, HTTP_HELP, http_params) { }
    ~HttpModule() { delete params; }
    bool begin(const char*, int, SnortConfig*) override;
    bool end(const char*, int, SnortConfig*) override;
    bool set(const char*, Value&, SnortConfig*) override;
    unsigned get_gid() const override { return HttpEnums::HTTP_GID; }
    const RuleMap* get_rules() const override { return http_events; }
    const HttpParaList* get_once_params()
    {
        HttpParaList* ret_val = params;
        params = nullptr;
        return ret_val;
    }

    const PegInfo* get_pegs() const override { return peg_names; }
    PegCount* get_counts() const override { return peg_counts; }
    static void increment_peg_counts(HttpEnums::PEG_COUNT counter)
        { peg_counts[counter]++; return; }

#ifdef REG_TEST
    static const PegInfo* get_peg_names() { return peg_names; }
    static const PegCount* get_peg_counts() { return peg_counts; }
    static void reset_peg_counts()
    {
        for (unsigned k=0; k < HttpEnums::PEG_COUNT_MAX; peg_counts[k++] = 0);
    }
#endif

private:
    static const Parameter http_params[];
    static const RuleMap http_events[];
    HttpParaList* params = nullptr;
    static const PegInfo peg_names[];
    static THREAD_LOCAL PegCount peg_counts[];
};

#endif

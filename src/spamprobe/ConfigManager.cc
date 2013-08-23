///###////////////////////////////////////////////////////////////////////////
//
// Burton Computer Corporation
// http://www.burton-computer.com
// http://www.cooldevtools.com
// $Id: ConfigManager.cc 272 2007-01-06 19:37:27Z brian $
//
// Copyright (C) 2007 Burton Computer Corporation
// ALL RIGHTS RESERVED
//
// This program is open source software; you can redistribute it
// and/or modify it under the terms of the Q Public License (QPL)
// version 1.0. Use of this software in whole or in part, including
// linking it (modified or unmodified) into other programs is
// subject to the terms of the QPL.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// Q Public License for more details.
//
// You should have received a copy of the Q Public License
// along with this program; see the file LICENSE.txt.  If not, visit
// the Burton Computer Corporation or CoolDevTools web site
// QPL pages at:
//
//    http://www.burton-computer.com/qpl.html
//    http://www.cooldevtools.com/qpl.html
//

#include <fstream>
#include "File.h"
#include "CommandConfig.h"
#include "DatabaseConfig.h"
#include "FilterConfig.h"
#include "ParserConfig.h"
#include "HeaderPrefixList.h"
#include "HdlToken.h"
#include "HdlParser.h"
#include "HdlPrinter.h"
#include "HdlStatement.h"
#include "HdlToken.h"
#include "HdlSyntaxChecker.h"
#include "ConfigManager.h"

static const char *syntax_src = 
"begin language spamprobe;"
"  begin block parser;"
"    args 0 0;"
"    begin statement replace_non_ascii;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin statement non_ascii_replacement;"
"      args 1 1 string;"
"    end;"
"    begin statement min_term_length;"
"      args 1 1 int;"
"    end;"
"    begin statement max_term_length;"
"      args 1 1 int;"
"    end;"
"    begin statement remove_html;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin statement min_phrase_terms;"
"      args 1 1 int;"
"    end;"
"    begin statement max_phrase_terms;"
"      args 1 1 int;"
"    end;"
"    begin statement min_phrase_chars;"
"      args 1 1 int;"
"    end;"
"    begin statement max_phrase_chars;"
"      args 1 1 int;"
"    end;"
"    begin statement max_message_terms;"
"      args 1 1 int;"
"    end;"
"    begin statement spamprobe_field_name;"
"      args 1 1 string;"
"    end;"
"    begin statement keep_suspicious_tags;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin statement ignore_body;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin block header;"
"      args 0 0;"
"      begin statement simple_prefix;"
"        args 1 1 string;"
"      end;"
"      begin statement full_prefix;"
"        args 3 3 string string string;"
"      end;"
"      begin statement blank_prefixes_mode;"
"        args 1 1 table;"
"        table id true id false;"
"      end;"
"      begin statement ignore_x_headers_mode;"
"        args 1 1 table;"
"        table id true id false;"
"      end;"
"      begin statement all_headers_mode;"
"        args 1 1 table;"
"        table id true id false;"
"      end;"
"      begin statement no_headers_mode;"
"        args 1 1 table;"
"        table id true id false;"
"      end;"
"    end;"
"  end;"
"  begin block filter;"
"    args 0 0;"
"    begin statement score_mode;"
"      args 1 1 table;"
"      table id original id alt1 id normal;"
"    end;"
"    begin statement good_bias;"
"      args 1 1 int;"
"    end;"
"    begin statement terms_for_score;"
"      args 1 1 int;"
"    end;"
"    begin statement max_word_repeats;"
"      args 1 1 int;"
"    end;"
"    begin statement min_word_count;"
"      args 1 1 int;"
"    end;"
"    begin statement extend_top_terms;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin statement min_array_size;"
"      args 1 1 int;"
"    end;"
"    begin statement water_counts;"
"      args 1 1 table;"
"      table id true id false;"
"    end;"
"    begin statement new_word_score;"
"      args 1 1 double;"
"    end;"
"    begin statement spam_threshold;"
"      args 1 1 double;"
"    end;"
"    begin statement min_distance_for_score;"
"      args 1 1 double;"
"    end;"
"  end;"
"  begin block database;"
"    args 0 0;"
"    begin statement type;"
"      args 1 1 table;"
"      table id default id bdb id pbl id hash id split;"
"    end;"
"    begin statement private_filename;"
"      args 1 1 string;"
"    end;"
"    begin statement shared_filename;"
"      args 1 1 string;"
"    end;"
"    begin statement target_size_mb;"
"      args 1 1 int;"
"    end;"
"    begin statement max_cache_terms;"
"      args 1 1 int;"
"    end;"
"  end;"
"end;"
;

static const char *mode_to_string(FilterConfig::ScoreMode mode)
{
  switch (mode) {
  case FilterConfig::SCORE_ORIGINAL:
    return "original";

  case FilterConfig::SCORE_ALT1:
    return "alt1";

  default:
    return "normal";
  }
}

static FilterConfig::ScoreMode string_to_mode(const string &str)
{
  if (str == "alt1") {
    return FilterConfig::SCORE_ALT1;
  }
  if (str == "original") {
    return FilterConfig::SCORE_ORIGINAL;
  }
  return FilterConfig::SCORE_NORMAL;
}

ConfigManager::ConfigManager()
  : m_commandConfig(new CommandConfig),
    m_databaseConfig(new DatabaseConfig),
    m_filterConfig(new FilterConfig),
    m_parserConfig(new ParserConfig)
{
  File basedir(File::getHomeDir(), ".spamprobe");
  m_configFile.set(new File(basedir, "spamprobe.hdl"));
}

ConfigManager::~ConfigManager()
{
}

void ConfigManager::loadFromFile(const File &config_file)
{
  HdlParser parser;
  Ref<HdlStatement> config_hdl(parser.parseFile(config_file));
  Ref<HdlStatement> config_syntax(parser.parseString(syntax_src));
  HdlSyntaxChecker checker(config_syntax);
  checker.checkSyntax(config_hdl);

  for (int i = 0; i < config_hdl->numChildren(); ++i) {
    const CRef<HdlStatement> &child = config_hdl->child(i);
    string name(child->name()->strValue());
    if (name == "parser") {
      loadParserConfig(child);
    } else if (name == "filter") {
      loadFilterConfig(child);
    } else if (name == "database") {
      loadDatabaseConfig(child);
    }
  }

  *m_configFile = config_file;
}

void ConfigManager::loadParserConfig(const CRef<HdlStatement> &config_hdl)
{
  const CRef<HdlToken> noarg;
  for (int i = 0; i < config_hdl->numChildren(); ++i) {
    const CRef<HdlStatement> &child = config_hdl->child(i);
    string name(child->name()->strValue());
    const CRef<HdlToken> &arg = (child->numArguments() > 0) ? child->argument(0) : noarg;
    if (name == "replace_non_ascii") {
      m_parserConfig->setReplaceNonAsciiChars(arg->isTrueValue() ? 'z' : 0);
    } else if (name == "non_ascii_replacement") {
      if (arg->strValue().length() > 0) {
        m_parserConfig->setReplaceNonAsciiChars(arg->strValue()[0]);
      }
    } else if (name == "min_term_length") {
      m_parserConfig->setMinTermLength(arg->intValue());
    } else if (name == "max_term_length") {
      m_parserConfig->setMaxTermLength(arg->intValue());
    } else if (name == "remove_html") {
      m_parserConfig->setRemoveHTML(arg->isTrueValue());
    } else if (name == "min_phrase_terms") {
      m_parserConfig->setMinPhraseTerms(arg->intValue());
    } else if (name == "max_phrase_terms") {
      m_parserConfig->setMaxPhraseTerms(arg->intValue());
    } else if (name == "min_phrase_chars") {
      m_parserConfig->setMinPhraseChars(arg->intValue());
    } else if (name == "max_phrase_chars") {
      m_parserConfig->setMaxPhraseChars(arg->intValue());
    } else if (name == "max_message_terms") {
      m_parserConfig->setMaxTermsPerMessage(arg->intValue());
    } else if (name == "spamprobe_field_name") {
      m_parserConfig->setSpamProbeFieldName(arg->strValue());
    } else if (name == "keep_suspicious_tags") {
      m_parserConfig->setKeepSuspiciousTags(arg->isTrueValue());
    } else if (name == "ignore_body") {
      m_parserConfig->setIgnoreBody(arg->isTrueValue());
    } else if (name == "header") {
      loadHeaderConfig(child);
    }
  }
}

void ConfigManager::loadHeaderConfig(const CRef<HdlStatement> &config_hdl)
{
  HeaderPrefixList *headers = m_parserConfig->headers();
  headers->setDefaultHeadersMode();

  bool headers_reset = false;

  const CRef<HdlToken> noarg;
  for (int i = 0; i < config_hdl->numChildren(); ++i) {
    const CRef<HdlStatement> &child = config_hdl->child(i);
    string name(child->name()->strValue());
    const CRef<HdlToken> &arg = (child->numArguments() > 0) ? child->argument(0) : noarg;
    if (name == "simple_prefix") {
      if (!headers_reset) {
        headers->setNoHeadersMode();
        headers_reset = true;
      }
      headers->addSimpleHeaderPrefix(arg->strValue());
    } else if (name == "full_prefix") {
      if (!headers_reset) {
        headers->setNoHeadersMode();
        headers_reset = true;
      }
      string first_prefix = child->argument(1)->strValue();
      string other_prefix = child->argument(2)->strValue();
      headers->addHeaderPrefix(arg->strValue(), first_prefix, other_prefix);
    } else if (name == "blank_prefixes_mode") {
      if (arg->isTrueValue()) {
        headers->setBlankPrefixesMode();
      }
    } else if (name == "ignore_x_headers_mode") {
      if (arg->isTrueValue()) {
        headers->setNonXHeadersMode();
        headers_reset = true;
      }
    } else if (name == "all_headers_mode") {
      if (arg->isTrueValue()) {
        headers->setAllHeadersMode();
        headers_reset = true;
      }
    } else if (name == "no_headers_mode") {
      if (arg->isTrueValue()) {
        headers->setNoHeadersMode();
        headers_reset = true;
      }
    }
  }
}

void ConfigManager::loadFilterConfig(const CRef<HdlStatement> &config_hdl)
{
  const CRef<HdlToken> noarg;
  for (int i = 0, limit = config_hdl->numChildren(); i < limit; ++i) {
    const CRef<HdlStatement> &child = config_hdl->child(i);
    string name(child->name()->strValue());
    const CRef<HdlToken> &arg = (child->numArguments() > 0) ? child->argument(0) : noarg;
    if (name == "score_mode") {
      m_filterConfig->setScoreMode(string_to_mode(arg->strValue()));
    } else if (name == "good_bias") {
      m_filterConfig->setGoodBias(arg->intValue());
    } else if (name == "terms_for_score") {
      m_filterConfig->setTermsForScore(arg->intValue());
    } else if (name == "max_word_repeats") {
      m_filterConfig->setMaxWordRepeats(arg->intValue());
    } else if (name == "min_word_count") {
      m_filterConfig->setMinWordCount(arg->intValue());
    } else if (name == "extend_top_terms") {
      m_filterConfig->setExtendTopTerms(arg->isTrueValue());
    } else if (name == "min_array_size") {
      m_filterConfig->setMinArraySize(arg->intValue());
    } else if (name == "water_counts") {
      m_filterConfig->setWaterCounts(arg->isTrueValue());
    } else if (name == "new_word_score") {
      m_filterConfig->setNewWordScore(arg->doubleValue());
    } else if (name == "spam_threshold") {
      m_filterConfig->setSpamThreshold(arg->doubleValue());
    } else if (name == "min_distance_for_score") {
      m_filterConfig->setMinDistanceForScore(arg->doubleValue());
    }
  }
}

void ConfigManager::writeToFile(const File &config_file) const
{
  ofstream out(config_file.getPath().c_str());
  HdlPrinter printer;
  writeConfig(printer, out);
}

void ConfigManager::writeConfig(HdlPrinter &printer,
                                ostream &out) const
{
  printer.beginBlock(out);
  printer.beginStatement(out, "spamprobe");
  printer.endStatement(out);
  writeParserConfig(printer, out);
  writeFilterConfig(printer, out);
  writeDatabaseConfig(printer, out);
  printer.endBlock(out);
}

void ConfigManager::writeParserConfig(HdlPrinter &printer,
                                      ostream &out) const
{
  printer.beginBlock(out);
  printer.beginStatement(out, "parser");
  printer.endStatement(out);

  printer.printBoolStatement(out, "replace_non_ascii", m_parserConfig->getReplaceNonAsciiChars());
  if (m_parserConfig->getReplaceNonAsciiChars()) {
    printer.printStringStatement(out, "non_ascii_replacement", m_parserConfig->getNonAsciiCharReplacement());
  }
  printer.printIntStatement(out, "min_term_length", m_parserConfig->getMinTermLength());
  printer.printIntStatement(out, "max_term_length", m_parserConfig->getMaxTermLength());
  printer.printBoolStatement(out, "remove_html", m_parserConfig->getRemoveHTML());
  printer.printIntStatement(out, "min_phrase_terms", m_parserConfig->getMinPhraseTerms());
  printer.printIntStatement(out, "max_phrase_terms", m_parserConfig->getMaxPhraseTerms());
  printer.printIntStatement(out, "max_phrase_chars", m_parserConfig->getMaxPhraseChars());
  printer.printIntStatement(out, "min_phrase_chars", m_parserConfig->getMinPhraseChars());
  printer.printIntStatement(out, "max_message_terms", m_parserConfig->getMaxTermsPerMessage());
  printer.printStringStatement(out, "spamprobe_field_name", m_parserConfig->spamprobeFieldName());
  printer.printBoolStatement(out, "keep_suspicious_tags", m_parserConfig->getKeepSuspiciousTags());
  printer.printBoolStatement(out, "ignore_body", m_parserConfig->getIgnoreBody());

  printer.beginBlock(out);
  printer.beginStatement(out, "header");
  printer.endStatement(out);

  HeaderPrefixList *headers = m_parserConfig->headers();
  printer.printBoolStatement(out, "blank_prefixes_mode", headers->getBlankPrefixesMode());
  printer.printBoolStatement(out, "ignore_x_headers_mode", headers->getNonXHeadersMode());
  printer.printBoolStatement(out, "all_headers_mode", headers->getAllHeadersMode());
  printer.printBoolStatement(out, "no_headers_mode", headers->getNoHeadersMode());

  if (!headers->getAllHeadersMode()) {
    for (HeaderPrefixList::const_iterator i = headers->begin(); i != headers->end(); ++i) {
      printer.beginStatement(out, "full_prefix");
      printer.addString(out, i.name());
      printer.addString(out, i.firstPrefix());
      printer.addString(out, i.otherPrefix());
      printer.endStatement(out);
    }
  }

  printer.endBlock(out); // header
  printer.endBlock(out); // parser
}

void ConfigManager::writeFilterConfig(HdlPrinter &printer,
                                      ostream &out) const
{
  const FilterConfig *config = filterConfig();

  printer.beginBlock(out);
  printer.beginStatement(out, "filter");
  printer.endStatement(out);

  printer.printIDStatement(out, "score_mode", mode_to_string(config->getScoreMode()));
  printer.printIntStatement(out, "good_bias", config->getGoodBias());
  printer.printIntStatement(out, "terms_for_score", config->getTermsForScore());
  printer.printIntStatement(out, "max_word_repeats", config->getMaxWordRepeats());
  printer.printIntStatement(out, "min_word_count", config->getMinWordCount());
  printer.printBoolStatement(out, "extend_top_terms", config->getExtendTopTerms());
  printer.printIntStatement(out, "min_array_size", config->getMinArraySize());
  printer.printBoolStatement(out, "water_counts", config->getWaterCounts());
  printer.printDoubleStatement(out, "new_word_score", config->getNewWordScore());
  printer.printDoubleStatement(out, "spam_threshold", config->getSpamThreshold());
  printer.printDoubleStatement(out, "min_distance_for_score", config->getMinDistanceForScore());

  printer.endBlock(out); // filter
}

void ConfigManager::loadDatabaseConfig(const CRef<HdlStatement> &config_hdl)
{
  const CRef<HdlToken> noarg;
  for (int i = 0, limit = config_hdl->numChildren(); i < limit; ++i) {
    const CRef<HdlStatement> &child = config_hdl->child(i);
    string name(child->name()->strValue());
    const CRef<HdlToken> &arg = (child->numArguments() > 0) ? child->argument(0) : noarg;
    if (name == "type") {
      string type_name(arg->strValue());
      if (type_name == "default") {
        type_name = "";
      }
      m_databaseConfig->setDatabaseType(type_name);
    } else if (name == "private_filename") {
      m_databaseConfig->setPrivateFilename(arg->strValue());
    } else if (name == "shared_filename") {
      m_databaseConfig->setSharedFilename(arg->strValue());
    } else if (name == "target_size_mb") {
      m_databaseConfig->setTargetSizeMB(arg->intValue());
    } else if (name == "max_cache_terms") {
      m_databaseConfig->setMaxCacheTerms(arg->intValue());
    }
  }
}

void ConfigManager::writeDatabaseConfig(HdlPrinter &printer,
                                        ostream &out) const
{
  const DatabaseConfig *config = databaseConfig();

  printer.beginBlock(out);
  printer.beginStatement(out, "database");
  printer.endStatement(out);

  printer.printIDStatement(out, "type", config->databaseType().length() == 0 ? "default" : config->databaseType());
  printer.printStringStatement(out, "private_filename", config->privateFilename());
  printer.printStringStatement(out, "shared_filename", config->sharedFilename());
  printer.printIntStatement(out, "target_size_mb", config->targetSizeMB());
  printer.printIntStatement(out, "max_cache_terms", config->maxCacheTerms());

  printer.endBlock(out); // database
}

#include "Core.hpp"
#include "Exception.hpp"

#include <iostream>

using namespace Fas;

Core::Core () 
  : signature ( 0x1A736166 )
{
  
}


void 
Core::load ( const std::string& fileName ) 
{
  this->fileName = fileName;
  load ();
}

void 
Core::load () 
{
  try {
    open ();
    loadHeader ();
    loadFasSymbols ();
    deleteUndefinedSymbols ();
    deleteAssemblyTimeVariable ();
    deleteCannotBeForwardReferenced ();
    deleteNegativeSymbols ();
    deleteSpecialMarkers ();
    deleteAnonymousSymbols ();
  }
  catch ( std::exception& e ) {
    std::cerr << e.what () << std::endl;  
  }
}

void 
Core::open () 
{
  ifs.open ( fileName, std::ios::binary );
  if ( !ifs.is_open () ) {
    throw Exception ( "*.fas file not loaded." );
  }
}

void 
Core::loadHeader () 
{
  ifs.seekg (0);
  ifs.read ( (char*)&header, sizeof ( Header ) );
  if ( !ifs.good () ) {
    throw Exception ( "*.fas Header not loaded." );
  }
  if ( header.signature != signature ) {
    throw Exception ( "*.fas signature fail" );
  }
  if ( header.lengthOfHeader != 64 ) {
    throw Exception ( "*.fas header size not supported" );
  }
}

void 
Core::loadFasSymbols () 
{
  ifs.seekg ( header.offsetOfSymbolsTable );

  auto size = header.lengthOfSymbolsTable;
  auto count = size/sizeof ( Fas::Symbol );
  fasSymbols.resize ( count );
  for ( auto symbol : fasSymbols ) {
    symbol = loadFasSymbol ();
  }
}

Fas::Symbol
Core::loadFasSymbol () 
{
  Symbol symbol;
  ifs.read ( (char*)&symbol, sizeof ( Symbol ) );
  if ( !ifs.good () ) {
    throw Exception ( "*.fas symbol not loaded" );
  }

  return std::move ( symbol );
}

void 
Core::deleteUndefinedSymbols () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    if ( !it->wasDefined ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
}

void 
Core::deleteAssemblyTimeVariable () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    if ( it->assemblyTimeVariable ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
  
}

void 
Core::deleteCannotBeForwardReferenced () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    if ( it->cannotBeForwardReferenced ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
}

void 
Core::deleteSpecialMarkers () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    if ( it->specialMarker ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
}

void 
Core::deleteNegativeSymbols () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    if ( it->valueSign ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
}

void 
Core::deleteAnonymousSymbols () 
{
  auto it = std::begin ( fasSymbols );
  while ( it != std::end ( fasSymbols ) ) {
    bool isAnonymous = it->preprocessedSign ==0 && it->preprocessed == 0; 
    if ( isAnonymous ) {
      it = fasSymbols.erase ( it );
    }
    else {
      ++it;
    }
  }
}

void 
Core::loadSymbols () 
{
  for ( auto fasSymbol : fasSymbols ) {
    checkAbsoluteValue ( fasSymbol );
    auto symbol = loadSymbolFromFasSymbol ( fasSymbol );
    symbols.push_back ( symbol );
  }
}

void 
Core::checkAbsoluteValue ( Fas::Symbol& fasSymbol ) 
{
  if ( fasSymbol.typeOfValue != ValueTypes::ABSOLUTE_VALUE ) {
    throw Exception ( " Support only absolute value" );
  }
}

PluginSymbol 
Core::loadSymbolFromFasSymbol ( Fas::Symbol& fasSymbol ) 
{
  PluginSymbol symbol;

  symbol.value = fasSymbol.value;
  if ( fasSymbol.preprocessedSign ) {
    // in the strings table
  }
  else {
    // in the preprocessed pascal style
  }

  return std::move ( symbol );
}

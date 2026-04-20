#ifndef SPANIO_H
#define SPANIO_H

#include <string>
#include <vector>
#include "spanFile.h"

namespace SpanIO {

// .span reader (header + binary payload + derived channels).
bool readSpanFile(const std::string &path, spanFile &data);

// Rewrite "Config:" line (header line 7) for a given .span file.
void updateConfigLine(const std::string &path,
                      const std::vector<SensorConfig> &newConfigs);

// Rewrite "Recipe:" line (header line 8) to an exact body.
void overwriteRecipeLine(const std::string &path,
                         const std::string &recipeBody);

// Read only the body after "Recipe:" from the header (or " None" if missing).
std::string readRecipeBody(const std::string &path);

// Add/replace one derived-channel entry in the Recipe body
void updateRecipeEntry(const std::string &spanPath,
                       const std::string &derivedName,
                       const std::string &signal1,
                       const std::string &signal2,
                       const std::string &option);

} // namespace SpanIO

#endif // SPANIO_H

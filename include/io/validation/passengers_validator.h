#ifndef PASSENGERS_VALIDATOR_H
#define PASSENGERS_VALIDATOR_H

#include <glib.h>

/**
 * @brief Checks if a document number is valid (format).
 */
gboolean checkDocumentNo(const gchar *no);

/**
 * @brief Comparison function for documents.
 */
gint compareDocuments(const void *a, const void *b);

/**
 * @brief Checks if a passenger document number exists in the list.
 */
gboolean checkDocumentNumber(const gchar *docNumber, GArray *passengers);

/**
 * @brief Checks if a passenger gender is valid.
 */
gboolean checkPassangerGender(const gchar *gender);

#endif
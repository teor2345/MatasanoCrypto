//
//  safeint.h
//  MatasanoCrypto
//
//  Created by Tim Wilson-Brown on 17/01/2016.
//  Copyright © 2016 teor.
//  GPLv3
//

#ifndef safeint_h
#define safeint_h

#include <stdbool.h>
#include <stddef.h>

bool checked_add(size_t a, size_t b, size_t *rp);

#endif /* safeint_h */

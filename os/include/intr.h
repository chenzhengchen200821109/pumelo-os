#ifndef _INTR_H__
#define _INTR_H__

enum intr_status
{
    INTR_OFF,
    INTR_ON
};

enum intr_status intr_enable(void);
enum intr_status intr_disable(void);

enum intr_status get_intr_status();
enum intr_status set_intr_status(enum intr_status status);

#endif /* _INTR_H__ */


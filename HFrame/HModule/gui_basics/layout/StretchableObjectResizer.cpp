/*
  ==============================================================================

   This file is part of the H library.
   Copyright (c) 2017 - ROLI Ltd.

   H is an open source library subject to commercial or open-source
   licensing.

   By using H, you agree to the terms of both the H 5 End-User License
   Agreement and H 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.H.com/H-5-licence
   Privacy Policy: www.H.com/H-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   H IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

StretchableObjectResizer::StretchableObjectResizer() {}
StretchableObjectResizer::~StretchableObjectResizer() {}

void StretchableObjectResizer::addItem (const double size,
                                        const double minSize, const double maxSize,
                                        const int order)
{
    // the order must be >= 0 but less than the maximum integer value.
    HAssert (order >= 0 && order < std::numeric_limits<int>::max());
    HAssert (maxSize >= minSize);

    Item item;
    item.size = size;
    item.minSize = minSize;
    item.maxSize = maxSize;
    item.order = order;
    items.add (item);
}

double StretchableObjectResizer::getItemSize (const int index) const noexcept
{
    return isPositiveAndBelow (index, items.size()) ? items.getReference (index).size
                                                    : 0.0;
}

void StretchableObjectResizer::resizeToFit (const double targetSize)
{
    int order = 0;

    for (;;)
    {
        double currentSize = 0;
        double minSize = 0;
        double maxSize = 0;

        int nextHighestOrder = std::numeric_limits<int>::max();

        for (int i = 0; i < items.size(); ++i)
        {
            const Item& it = items.getReference(i);
            currentSize += it.size;

            if (it.order <= order)
            {
                minSize += it.minSize;
                maxSize += it.maxSize;
            }
            else
            {
                minSize += it.size;
                maxSize += it.size;
                nextHighestOrder = jmin (nextHighestOrder, it.order);
            }
        }

        const double thisIterationTarget = jlimit (minSize, maxSize, targetSize);

        if (thisIterationTarget >= currentSize)
        {
            const double availableExtraSpace = maxSize - currentSize;
            const double targetAmountOfExtraSpace = thisIterationTarget - currentSize;
            const double scale = availableExtraSpace > 0 ? targetAmountOfExtraSpace / availableExtraSpace : 1.0;

            for (int i = 0; i < items.size(); ++i)
            {
                Item& it = items.getReference(i);

                if (it.order <= order)
                    it.size = jlimit (it.minSize, it.maxSize, it.size + (it.maxSize - it.size) * scale);
            }
        }
        else
        {
            const double amountOfSlack = currentSize - minSize;
            const double targetAmountOfSlack = thisIterationTarget - minSize;
            const double scale = targetAmountOfSlack / amountOfSlack;

            for (int i = 0; i < items.size(); ++i)
            {
                Item& it = items.getReference(i);

                if (it.order <= order)
                    it.size = jmax (it.minSize, it.minSize + (it.size - it.minSize) * scale);
            }
        }

        if (nextHighestOrder < std::numeric_limits<int>::max())
            order = nextHighestOrder;
        else
            break;
    }
}


